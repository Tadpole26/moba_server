#include "pch.h"
#include "reconn_event.h"
#include <event2/event.h>
#include <event2/bufferevent.h>
#include "listen_thread.h"
#include "logic_interface.h"
#include "event_thread.h"
#include "io_thread.h"
#include "thread_dispatcher.h"
#include "net_opt.h"
#include "log_mgr.h"

void connect_cb(struct bufferevent* bev, short events, void* arg)
{
    CReconnEvent* pReConn = (CReconnEvent*)arg;
    pReConn->on_connect(events);
}

CReconnEvent::CReconnEvent(CEventThread* pThread,
    CThreadDispatcher* pDispatcher, int iSecs, uint32 maxBuffer)
    : m_pOwner(nullptr)
    , m_pBuffer(nullptr)
    , m_pThread(pThread)
    , m_pDispatcher(pDispatcher)
    , m_nPort(0)
    , m_bConnected(false)
    , m_bConnecting(false)
    , m_maxBuffer(maxBuffer)
{
    m_tmrConnect.Init(iSecs * 1000);
}

CReconnEvent::~CReconnEvent()
{
    free_bufferevent();
}

void CReconnEvent::connect()
{
    if (m_bConnected || m_bConnecting)
        return;

    if (m_pOwner == nullptr)
        return;

    //if (m_nConnectNum % 60 == 0)
    ++m_nConnectAll;
    ++m_nConnectNum;

    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(m_strIp.c_str());
    m_addr.sin_port = htons(m_nPort);
    m_pOwner->set_ip(m_addr.sin_addr.s_addr);

    if (m_pBuffer == nullptr)
        m_pBuffer = bufferevent_socket_new(m_pThread->Base(), -1, BEV_OPT_CLOSE_ON_FREE);

    if (m_pBuffer == nullptr)
        return;

    bufferevent_setcb(m_pBuffer, nullptr, nullptr, connect_cb, (void*)this);

    if (bufferevent_socket_connect(m_pBuffer, (sockaddr*)&m_addr, sizeof(m_addr)) < 0)
    {
        free_bufferevent();
        return;
    }
    m_bConnecting = true;
}


void CReconnEvent::on_connect(short events)
{
    m_bConnecting = false;
    m_bConnected = (events & BEV_EVENT_CONNECTED) != 0;

    std::string name;
    if (m_pOwner != nullptr)
        name = typeid(*m_pOwner).name();

    if (m_bConnected)
    {
        m_nConnectNum = 0;
        evutil_socket_t fd = bufferevent_getfd(m_pBuffer);
        if (fd != -1)
            m_pDispatcher->dispatch_accpet(fd, m_pOwner->get_soid()
                , *(sockaddr*)(&m_addr), m_maxBuffer, ct_connect);
        else
        {
            m_bConnected = false;
            Log_Error("%s connect to (%s:%u)(no=%d) times:%u alltimes=%u failed", name.c_str(), m_strIp.c_str(), (uint32)m_nPort, events, m_nConnectNum, m_nConnectAll);
        }
    }
    else
        free_bufferevent();
}

void CReconnEvent::on_disconnect()
{
    m_bConnected = false;
    m_bConnecting = false;

    free_bufferevent();
}

void CReconnEvent::stoptimer()
{
    m_bTimerStop = true;
}

void CReconnEvent::trigger(uint32 dwTM)
{
    if (m_bTimerStop)
        return;

    if (!m_tmrConnect.On(dwTM))
        return;

    if (!m_bConnected && !m_bConnecting)
    {
        m_pOwner->connecttimes(m_nConnectNum);
        m_trigger = 0;
        connect();
    }
    else
    {
        if (m_bConnected)
        {
            ++m_trigger;
            if (m_trigger % 30 == 0)
            {
                m_pOwner->trigger();
                m_trigger = 0;
            }
        }
    }
}

void CReconnEvent::init(const std::string& strIp, uint16 nPort)
{
    m_strIp = strIp;
    m_nPort = nPort;

    connect();
}

void CReconnEvent::free_bufferevent()
{
    if (m_pBuffer)
    {
        bufferevent_free(m_pBuffer);
        m_pBuffer = nullptr;
    }
}








