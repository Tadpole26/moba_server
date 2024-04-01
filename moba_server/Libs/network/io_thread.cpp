#include "pch.h"
#include "io_thread.h"
#include <event2/event.h>
#include <event2/event_struct.h>
#include "net_connection.h"
#include "msg_interface.h"
#include "msg_queue.h"
#include "logic_interface.h"
#include "thread_dispatcher.h"
#include "net_opt.h"
#include "log_mgr.h"

CIoThread::CIoThread()
{
}

CIoThread::~CIoThread()
{
    Fini();
}

static fn_io_send_msg g_io_send_msg_fn = nullptr;
///////////////////////////////////////////////////////////////////////////
void regfn_io_send_msg(fn_io_send_msg fn)
{
    if (fn) g_io_send_msg_fn = fn;
}
//初始化io线程
int CIoThread::Init(thread_oid_t iOid, const io_thread_setting& conf,
    CThreadDispatcher* pDispatcher, CEventThread* pLogicThread)
{
    if (iOid == invalid_thread_oid || conf.m_ConnsOfIo <= 0)
    {
        Log_Error("init io thread oid error, oid:%u, conns:%u", iOid, conf.m_ConnsOfIo);
        return -1;
    }


    m_put_out_count = 0;
    m_get_out_count = 0;
    m_pEvThread = new CEventThread;
    if (m_pEvThread == nullptr)
        return -1;
    //单线程处理
    if (conf.MqType() == EMQ_NO)
    {
        if (m_pEvThread->Init(iOid, pLogicThread->Base()) != 0)
        {
            Log_Error("init io thread event thread error");
            return -1;
        }
    }
    else
    {
        if (m_pEvThread->Init(iOid) != 0)
        {
            Log_Error("init io thread event thread error");
            return -1;
        }
    }
    //设置最大连接数(bufferevent pool 最大数量)
    if (!m_connPool.init(this, conf.m_ConnsOfIo))
    {
        Log_Error("init io thread conn pool error, conns:%u", conf.m_ConnsOfIo);
        return -1;
    }

    /*
    * listen_thread监听到有人连接, 生成accept消息push到m_AcceptConnQueue中
    * 并调用socketpair的write触发socketpair的read回调
    * 从而触发m_AcceptConnQueue的pop函数,最后调用msg_accept_fn回调函数
    */
    if (!m_AcceptConnQueue.init(conf.m_acceptMaxSize, m_pEvThread->Base(),
        std::bind(&CIoThread::msg_accept_fn, this, std::placeholders::_1)
        , conf.MqType()))
    {
        Log_Error("init io thread accept queue error, accept max size:%u", conf.m_acceptMaxSize);
        return -1;
    }

    /*
    * listen_thread监听到有人连接,会往m_InputQueue中push连接消息
    * bufferevent触发read回到会往m_InputQueue中push数据消息
    * 最终调用msg_output_fn进行消息处理
    * 这里要注意,传入的base是logic的base,所以msg_output_fn实际是在logic线程中处理的
    */
    if (!(m_InputQueue.init(conf.m_inMaxSize, pLogicThread->Base(),
        std::bind(&CIoThread::msg_input_fn, this, std::placeholders::_1)
        , conf.MqType())))
    {
        Log_Error("init io thread input queue error, max size:%u", conf.m_inMaxSize);
        return -1;
    }

    //CLogicThread to io_thread
    if (!(m_OutputQueue.init(conf.m_outMaxSize, m_pEvThread->Base(),
        std::bind(&CIoThread::msg_output_fn, this, std::placeholders::_1)
        , conf.MqType())))
    {
        Log_Error("init io thread output queue error, max size:%u", conf.m_outMaxSize);
        return -1;
    }

    if (!m_connPool.start_timer(conf.m_nDetectSec))
    {
        Log_Error("init io thread timer error, time:%u", conf.m_nDetectSec);
        return -1;
    }

    m_pParent = pDispatcher;
    m_setting = conf;
    m_bStop = false;
    return 0;
}

void CIoThread::Fini()
{
    m_bStop = true;
    m_pParent = nullptr;
    m_connPool.release();

    m_AcceptConnQueue.release();
    m_InputQueue.release();
    m_OutputQueue.release();
    if (m_pEvThread != nullptr)
        delete m_pEvThread;
    m_pEvThread = nullptr;
}

void CIoThread::Stop()
{
    if (m_bStop) return;
    m_bStop = true;

    m_AcceptConnQueue.stop();
    m_InputQueue.stop();
    m_OutputQueue.stop();

    if (m_pEvThread != nullptr)
        m_pEvThread->Stop();
}

CEventThread* CIoThread::EvThread()
{
    return m_pEvThread;
}
//listen到消息,生成NetConn(bufferevent),并调用io_notify_logic_conn_msg
void CIoThread::msg_accept_fn(const event_msg_t msg)
{
    tagAcceptConnMsg* pAccept = (tagAcceptConnMsg*)msg;
    if (pAccept == nullptr)
        return;

    CNetConn* pNewConn = ConnPool()->newconn(pAccept->m_maxBuffer);
    if (pNewConn == nullptr)
    {
        evutil_closesocket(pAccept->m_fd);
        Log_Error("the conn count is over limit ip = %s", CNetOpt::Inet_Ip(pAccept->m_addr).c_str());
        return;
    }

    if (pNewConn->init(pAccept->m_fd, EvThread()->Base()
        , pAccept->m_addr, pAccept->m_connType, m_setting.m_nSecRecvLimit) != 0)
    {
        Log_Error("net_conn_init failed ip = %s", CNetOpt::Inet_Ip(pAccept->m_addr).c_str());
        evutil_closesocket(pAccept->m_fd);
        ConnPool()->delconn(pNewConn->get_conn_id());
        return;
    }

    EConn_State state = cs_net_accepted;
    if (pAccept->m_connType == ct_connect)
        state = cs_net_connected;

    pNewConn->recv_sec();
    //生成logic conn消息push到m_InputQueue中,调用m_InputQueue回调函数msg_input_fn
    if (io_notify_logic_conn_msg(pNewConn, state, pAccept->m_soid) == -1)
    {
        Log_Error("io_notify failed ip = %s", CNetOpt::Inet_Ip(pAccept->m_addr).c_str());
        ConnPool()->delconn(pNewConn->get_conn_id());
    }
    Log_Custom("accept", "net_conn_init connect ip = %s, conn type = %d", 
        CNetOpt::Inet_Ip(pAccept->m_addr).c_str(), pAccept->m_connType);
}


void CIoThread::io_handle_conn_msg(tagConnMsg* pConnMsg)
{
    if (pConnMsg->m_body.m_state == cs_layer_disconn)
        ConnPool()->delconn(pConnMsg->m_hd.m_connOid);
    else if (pConnMsg->m_body.m_state == cs_layer_kill)
        ConnPool()->killconn(pConnMsg->m_hd.m_connOid);
    else if (pConnMsg->m_body.m_state == cs_layer_connected)
    {
        CNetConn* pConn = ConnPool()->findconn(pConnMsg->m_hd.m_connOid);
        if (pConn)
            pConn->set_max_buffer(pConnMsg->m_body.m_maxBuffer);
    }
    else if (pConnMsg->m_body.m_state == cs_net_exit)
        Stop();
}

void CIoThread::msg_input_fn(const event_msg_t msg)
{
    CLogicInterface* pLogicIF = GetParent()->get_Logic();

    tagHostHd* pHostMsgHd = (tagHostHd*)msg;
    //连接消息
    if (pHostMsgHd->m_type == HMT_CONN)
    {
        tagConnMsg* pConnMsg = (tagConnMsg*)msg;
        pLogicIF->handle_conn_msg(pConnMsg);
    }
    //普通消息
    else
        pLogicIF->handle_logic_msgv(msg);
}

void CIoThread::msg_output_fn(const event_msg_t msg)
{
    tagHostHd* pHostMsgHd = (tagHostHd*)msg;
    if (pHostMsgHd->m_type == HMT_CONN)
    {
        tagConnMsg* pConnMsg = (tagConnMsg*)msg;
        io_handle_conn_msg(pConnMsg);
    }
    else
    {
        AddGet();
        if (g_io_send_msg_fn != nullptr)
            (*g_io_send_msg_fn)(msg, this);
    }
}

CMsgQueue& CIoThread::InPut()
{
    return m_InputQueue;
}

CMsgQueue& CIoThread::OutPut()
{
    return m_OutputQueue;
}

CMsgQueue& CIoThread::AcceptPut()
{
    return m_AcceptConnQueue;
}

CNetConnPool* CIoThread::ConnPool()
{
    return &m_connPool;
}

CThreadDispatcher* CIoThread::GetParent()
{
    return m_pParent;
}






















