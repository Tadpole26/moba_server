#include "pch.h"
#include "logic_interface.h"
#include "listen_thread.h"
#include "reconn_event.h"
#include "msg_queue.h"
#include "msg_interface.h"
#include "net_connection.h"
#include "thread_dispatcher.h"
#include "io_thread.h"
#include "net_opt.h"
#include "../platform/log_mgr.h"

CLogicSession::CLogicSession()
    : m_soid(invalid_session_oid)
    , m_state(ss_unknown)
    , m_ip(0)
{
    m_hd.m_threadOid = invalid_thread_oid;
    m_hd.m_connOid = invalid_conn_oid;
    m_strip.clear();
}

CLogicSession::~CLogicSession()
{
}

void CLogicSession::accpeted()
{
    m_state = ss_conn_reg_ok;
    on_accept();
}

void CLogicSession::connected()
{
    m_state = ss_conn_reg_ok;
    on_connect();
}

void CLogicSession::disconnected()
{
    m_tmDiscon = GetCurrTime();
    m_state = ss_conn_disconn;
    on_disconnect();
}

bool CLogicSession::is_connected()
{
    return m_state == ss_conn_reg_ok;
}

void CLogicSession::set_ip(uint32_t ip)
{
    m_ip = ip;
    m_strip = CNetOpt::Inet_Ip(ip);
}

///////////////////////////////////////////////////////////////////////////
CReconnSession::CReconnSession()
    : m_pReconn(nullptr)
{
    m_type = ct_connect;
}

CReconnSession::~CReconnSession()
{
    if (m_pReconn != nullptr)
        delete m_pReconn;
    m_pReconn = nullptr;
}

void CReconnSession::disconnected()
{
    m_pReconn->on_disconnect();
    CLogicSession::disconnected();
}

void CReconnSession::stop()
{
    if (m_pReconn != nullptr)
        m_pReconn->stoptimer();
}

std::string CReconnSession::GetReconIp()
{
    return m_pReconn->GetIp();
}

uint16_t CReconnSession::GetReconPort()
{
    return m_pReconn->GetPort();
}

void CReconnSession::set_reconn_event(CReconnEvent* pReconn)
{
    pReconn->set_owner(this);
    m_pReconn = pReconn;
}

///////////////////////////////////////////////////////////////////////////
session_oid_t CLogicInterface::m_nPreNum = 0;
CLogicInterface::CLogicInterface()
    : m_pDispatcher(nullptr)
{
}

CLogicInterface::~CLogicInterface()
{
}

void CLogicInterface::initialize(CThreadDispatcher* pDispatcher, thread_oid_t tNum, conn_oid_t conNum)
{
    m_pDispatcher = pDispatcher;
    m_sessionVecs.resize(tNum);
    for (int i = 0; i < (int)m_sessionVecs.size(); ++i)
    {
        m_sessionVecs[i].resize(conNum);
    }
}

CLogicSession* CLogicInterface::new_server_session()
{
    return nullptr;
}

void CLogicInterface::handle_conn_msg(const tagConnMsg* pConnMsg)
{
    const tagConnMsgBody& body = pConnMsg->m_body;
    const tagHostHd& hd = pConnMsg->m_hd;
    CLogicSession* pSession = nullptr;
    switch (body.m_state)
    {
    case cs_net_connected:
        pSession = get_pre_session(body.m_soid);
        if (pSession)
        {
            add_session(pSession, hd, 0);
            pSession->connected();
        }
        break;
    case cs_net_disconn:
        pSession = get_session(hd);
        if (pSession)
        {
            remove_session(pSession);
            send_io_conn_msg(hd, cs_layer_disconn, pSession->get_soid(), 0);
            pSession->disconnected();
        }
        else
            send_io_conn_msg(hd, cs_layer_disconn, -1, 0);
        break;
    case cs_net_norecv:
        pSession = get_session(hd);
        if (pSession)
        {
            pSession->SetTimeOut(true);
            remove_session(pSession);
            pSession->disconnected();
        }
        break;
    case cs_net_accepted:
        pSession = new_server_session();
        if (pSession)
        {
            add_session(pSession, hd, 0);
            pSession->on_prepared();
        }
        break;
    default:
        break;
    }
}

void CLogicInterface::handle_logic_msgv(const void* pMsg)
{
    tagNetMsg* pNetMsg = (tagNetMsg*)pMsg;
    handle_logic_msg(pNetMsg);
}

void CLogicInterface::handle_logic_msg(const tagNetMsg* pNetMsg)
{
    CLogicSession* pSession = get_session(pNetMsg->m_hd);
    if (pSession && pNetMsg->m_body)
        pSession->handle_msgv((const void*)(pNetMsg->m_body));

}

void CLogicInterface::stop()
{
    for (auto itr = m_PreSession.begin(); itr != m_PreSession.end(); ++itr)
    {
        CLogicSession* pSession = itr->second;
        CReconnSession* pRecon = dynamic_cast<CReconnSession*>(pSession);
        if (pRecon != nullptr)
            pRecon->stop();
    }
}

session_oid_t CLogicInterface::add_pre_session(CReconnSession* pSession)
{
    pSession->set_dispatcher(m_pDispatcher);
    pSession->set_soid(++m_nPreNum);
    m_PreSession.insert(std::make_pair(pSession->get_soid(), pSession));
    return m_nPreNum;
}

CReconnSession* CLogicInterface::get_pre_session(session_oid_t id)
{
    auto itr = m_PreSession.find(id);
    if (itr != m_PreSession.end())
        return itr->second;
    return nullptr;
}

int CLogicInterface::kill_pre_session(CReconnSession* pSession)
{
    send_io_conn_msg(pSession->get_hd(), cs_layer_kill, pSession->get_soid(), 0);
    remove_session(pSession);
    pSession->on_disconnect();
    m_PreSession.erase(pSession->get_soid());
    return 0;
}

int CLogicInterface::add_session(CLogicSession* pSession, const tagHostHd& hd, uint32_t maxBuffer)
{
    pSession->set_dispatcher(m_pDispatcher);

    if (!has_session(hd))
    {
        pSession->set_hd(hd);
        m_sessionVecs[hd.m_threadOid][hd.m_connOid] = pSession;

        if (maxBuffer > 0)
            send_io_conn_msg(hd, cs_layer_connected, pSession->get_soid(), maxBuffer);

        CIoThread* ioThread = m_pDispatcher->get_io_thread(hd.m_threadOid);
        if (ioThread)
        {
            CNetConnPool* connPool = ioThread->ConnPool();
            if (connPool)
                pSession->set_ip(connPool->get_ip(hd.m_connOid));
        }
        return 0;
    }
    return -1;
}

int CLogicInterface::remove_session(CLogicSession* pSession)
{
    const tagHostHd& hd = pSession->get_hd();
    if (has_session(hd))
    {
        if (pSession == m_sessionVecs[hd.m_threadOid][hd.m_connOid])
        {
            m_sessionVecs[hd.m_threadOid][hd.m_connOid] = nullptr;
            return 0;
        }
    }
    return -1;
}

int CLogicInterface::kill_session(CLogicSession* pSession)
{
    send_io_conn_msg(pSession->get_hd(), cs_layer_kill, pSession->get_soid(), 0);
    remove_session(pSession);
    pSession->on_disconnect();
    return 0;
}

CLogicSession* CLogicInterface::get_session(const tagHostHd& hd)
{
    if (hd.m_threadOid >= 0 && hd.m_threadOid < (int)m_sessionVecs.size())
    {
        session_vec& sessionVec = m_sessionVecs[hd.m_threadOid];
        if (hd.m_connOid >= 0 && hd.m_connOid < (int)sessionVec.size())
            return sessionVec[hd.m_connOid];
    }
    return nullptr;
}

bool CLogicInterface::has_session(const tagHostHd& hd)
{
    return get_session(hd) != nullptr;
}

int CLogicInterface::send_io_conn_msg(const tagHostHd& hd, EConn_State state, session_oid_t soid, uint32_t maxBuffer)
{
    if (m_pDispatcher)
        return logic_notify_io_conn_msg(m_pDispatcher, hd, state, soid, maxBuffer);
    return -1;
}






