#include "pch.h"
#include "net_connection.h"
#include <errno.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include "listen_thread.h"
#include "id_alloctor.h"
#include "msg_interface.h"
#include "event_thread.h"
#include "io_thread.h"
#include "thread_dispatcher.h"
#include <functional>
#include "net_opt.h"
#include "msg_parser.h"
#include "../platform/log_mgr.h"

//收到消息分包处理函数
static fn_io_recv_msg g_io_recv_msg_fn = nullptr;
void regfn_io_recv_msg(fn_io_recv_msg fn)
{
    if (fn) g_io_recv_msg_fn = fn;
}

//bufferevent读回调
void CNetConn::conn_readcb(bufferevent* pBuffer, void* args)
{
    CNetConn* pConn = (CNetConn*)args;
    if (pConn == nullptr)
        return;

    CMsgQueue& InputQueue = pConn->get_pool()->iothread()->InPut();
    if (g_io_recv_msg_fn == nullptr) return;

    if (pConn->GetParseCode() == pmr_cus_kill)
        return;

    EParse_Msg_Relt relt = pmr_ok;

    if (pConn->get_ev_buffer() != pBuffer)
    {
        relt = pmr_buffer_error;
        pConn->SetParseCode(pmr_buffer_error);
        pConn->get_pool()->disconn(pConn->get_conn_id());
    }
    else
    {
        relt = (*g_io_recv_msg_fn)(pConn, &InputQueue);
    }

    if (relt != pmr_ok)
    {
        pConn->SetParseCode(relt);
        if (pConn->GetParseCode() != pmr_cus_kill)
            pConn->get_pool()->disconn(pConn->get_conn_id());
    }
}
//断开连接、出现错误等回调
void CNetConn::conn_eventcb(bufferevent* pBuffer, short events, void* args)
{
    CNetConn* pConn = (CNetConn*)args;
    if (pConn == nullptr)
        return;

    if (events & BEV_EVENT_EOF) {}
    else if (events & BEV_EVENT_ERROR) {}
    else {}

    pConn->SetParseCode(pmr_buffer_error);
    pConn->get_pool()->disconn(pConn->get_conn_id());
}

CNetConn::CNetConn(conn_oid_t iConnId, CNetConnPool* pConnPool)
{
    m_iConnId = iConnId;
    m_pConnPool = pConnPool;
}

CNetConn::~CNetConn()
{
    release();
}

int CNetConn::init(evutil_socket_t fd, event_base* pEvBase, sockaddr& addr, EConn_Type type
    , uint32_t nSecRecvLimit)
{
    //设置fd非阻塞
    if (!CNetOpt::SetNoBlock(fd)) {}
    //设置缓冲区不拷贝
    if (!CNetOpt::SetKeepALive(fd) || !CNetOpt::SetBufSize(fd, 128 * 1024))
        return -1;

    CNetOpt::SetCloseExec(fd);

    bufferevent* pBuffer = bufferevent_socket_new(pEvBase, fd, BEV_OPT_CLOSE_ON_FREE);
    if (pBuffer == nullptr) return -1;
    //设置bufferevent读、写、结束错误等回调
    bufferevent_setcb(pBuffer, conn_readcb, conn_writecb, conn_eventcb, this);
    bufferevent_enable(pBuffer, EV_READ | EV_WRITE);
    //设置bufferevent水位线
    bufferevent_setwatermark(pBuffer, EV_READ, NET_HEAD_SIZE, 128 * 1024);

    m_pBuffer = pBuffer;
    m_addr = addr;
    m_ip = ((sockaddr_in*)(&addr))->sin_addr.s_addr;
    m_type = type;
    m_nSecRecvLimit = nSecRecvLimit;
    m_socket = fd;
    m_tmLast = GetCurrTime();
    return 0;
}

int CNetConn::release()
{
    m_iConnId = invalid_conn_oid;
    m_type = ct_unknown;
    m_nMaxBufferSize = DEF_MAX_BUFFER;

    m_ip = 0;
    m_tmLast = 0;
    m_nSecRecvs = 0;
    m_nAllRecv = 0;
    m_eParseCode = pmr_ok;

    m_socket = -1;
    if (m_pBuffer != nullptr)
    {
        bufferevent_free(m_pBuffer);
        m_pBuffer = nullptr;
    }
    return 0;
}

thread_oid_t CNetConn::get_thread_id()
{ 
    return m_pConnPool->iothread()->EvThread()->Id(); 
}

CIoThread* CNetConn::get_thread() 
{ 
    return m_pConnPool->iothread(); 
}

bool CNetConn::recv_sec()
{
    time_t tmNow = GetCurrTime();
    if (tmNow != m_tmLast)
    {
        if (m_nSecRecvs >= m_nSecRecvLimit && m_nSecRecvLimit != 0)
            return false;

        m_nSecRecvs = 0;
        m_tmLast = tmNow;
    }

    ++m_nAllRecv;
    ++m_nSecRecvs;
    return true;
}

int CNetConn::write(const void* pData, size_t size)
{
    if (m_pBuffer == nullptr) return -1;

    evbuffer* pOutBuf = bufferevent_get_output(m_pBuffer);
    if (pOutBuf == nullptr) return -1;

    size_t outLength = evbuffer_get_length(pOutBuf);
    if (outLength >= get_max_buffer())
        return -1;

    return bufferevent_write(m_pBuffer, pData, size);
}

/////////////////////////////////////////////////////////////////////////////////////////

CNetConnPool::CNetConnPool()
{}

CNetConnPool::~CNetConnPool()
{
    release();
}
//初始化bufferevent pool最大数量
bool CNetConnPool::init(CIoThread* pThread, uint16_t nMaxConn)
{
    if (nMaxConn == 0)
        return false;

    _pConns = new CNetConn * [nMaxConn];
    memset(_pConns, 0, sizeof(_pConns[0]) * nMaxConn);
    _nMaxConns = nMaxConn;

    if (!(_IdAlloctor.Init(nMaxConn)))
        return false;

    _pIoThread = pThread;

    return true;
}

int CNetConnPool::release()
{
    _tvdetect.stop();
    _mapkill.clear();

    if (_pConns != nullptr)
    {
        for (conn_oid_t id = 0; id < _nMaxConns; ++id)
        {
            delconn(id);
        }
        delete[] _pConns;
        _pConns = nullptr;
    }
    _IdAlloctor.Release();
    _setID.clear();
    return 0;
}


bool CNetConnPool::start_timer(int nDetectSec)
{
    bool res = false;
    m_nDetectSec = nDetectSec;
    res = _tvdetect.init(_pIoThread->EvThread()->Base(), true
        , CHECK_DETECT_INTERVAL * 1000
        , std::bind(&CNetConnPool::OnDetect, this, std::placeholders::_1));

    return res;
}

CNetConn* CNetConnPool::findconn(conn_oid_t id)
{
    if (is_valid(id))
        return _pConns[id];

    return nullptr;
}

CNetConn* CNetConnPool::newconn(uint32_t maxBuffer)
{
    if (_IdAlloctor.Full())
        remove_killed();

    uint16 id = _IdAlloctor.AllocId();
    if (!is_valid(id))
        return nullptr;

    CNetConn* pConn = nullptr;
    pConn = _pConns[id];
    if (pConn == nullptr)
    {
        pConn = new CNetConn(id, this);
        _pConns[id] = pConn;
    }
    else
        pConn = new (pConn)CNetConn(id, this);

    if (maxBuffer > 0)
        pConn->set_max_buffer(maxBuffer);
    else
        pConn->set_max_buffer(DEF_MAX_BUFFER);

    _setID.insert(id);

    return pConn;
}

int CNetConnPool::get_ip(conn_oid_t id)
{
    CNetConn* con = findconn(id);
    if (con != nullptr)
        return con->get_ip();
    return 0;
}

size_t CNetConnPool::conn_size()
{
    return _IdAlloctor.Size();
}

bool CNetConnPool::Full()
{
    return _IdAlloctor.Full();
}

void CNetConnPool::remove_killed()
{
    //time_t tmNow = GetCurrTime();
    conn_oid_t conid = invalid_conn_oid;
    for (auto itr = _mapkill.begin(); itr != _mapkill.end();)
    {
        conid = itr->first;
        itr = _mapkill.erase(itr);
        delconn(conid);
    }
}

int CNetConnPool::delconn(conn_oid_t id)
{
    if (!is_valid(id))
        return -1;

    _setID.erase(id);
    _IdAlloctor.FreeId(id);
    CNetConn*& pConn = _pConns[id];
    if (pConn)
        pConn->release();
    _mapkill.erase(id);
    return 0;
}

int CNetConnPool::killconn(conn_oid_t id)
{
    CNetConn* pConn = findconn(id);
    if (pConn == nullptr) return -1;
    pConn->SetParseCode(pmr_cus_kill);
    _mapkill.insert(std::make_pair(id, GetCurrTime()));
    return 0;
}

int CNetConnPool::write(conn_oid_t id, const void* pData, size_t size)
{
    CNetConn* pConn = findconn(id);
    if (pConn && pConn->isvaid())
        return pConn->write(pData, size);
    else
        return -1;
}

int CNetConnPool::disconn(conn_oid_t id)
{
    CNetConn* pConn = findconn(id);
    if (pConn)
        return io_notify_logic_conn_msg(pConn, cs_net_disconn, invalid_session_oid);
    return -1;
}

void CNetConnPool::OnDetect(uint32_t dwTM)
{
    net_conn_detect(dwTM);
    net_conn_kill(dwTM);
}

void CNetConnPool::net_conn_detect(uint32_t dwTM)
{
    if (_pConns != nullptr)
    {
        CNetConn* pConn = nullptr;
        time_t tmNow = GetCurrTime();
        for (auto itr = _setID.begin(); itr != _setID.end();)
        {
            pConn = findconn(*itr);
            if (pConn && pConn->get_last_recv() != 0
                && abs(tmNow - pConn->get_last_recv()) >= m_nDetectSec)
                itr = _setID.erase(itr);
            else
                ++itr;
        }
    }
}

void CNetConnPool::net_conn_kill(uint32_t dwTM)
{
    time_t tmNow = GetCurrTime();
    conn_oid_t conid = invalid_conn_oid;
    for (auto itr = _mapkill.begin(); itr != _mapkill.end();)
    {
        if (abs(itr->second + 20) <= tmNow)
        {
            conid = itr->first;
            itr = _mapkill.erase(itr);
            delconn(conid);
        }
        else
            ++itr;
    }
}
