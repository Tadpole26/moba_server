#pragma once
#include <string>
#include "net_define.h"
#include "time_event.h"
#include "../platform/util_time.h"

struct bufferevent;
class CEventThread;
class CThreadDispatcher;
class CReconnSession;
class CReconnEvent
{
public:
    CReconnEvent(CEventThread* pThread,
        CThreadDispatcher* pDispatcher,
        int iSecs, uint32 maxBuffer);
    virtual ~CReconnEvent();

    void trigger(uint32 dwTM);
    void connect();

    void init(const std::string& strIp, uint16 nPort);
    void on_connect(short events);
    void on_disconnect();
    bool is_connected() const { return m_bConnected; }
    void set_owner(CReconnSession* pOwner) { m_pOwner = pOwner; }
    void stoptimer();

    std::string GetIp() const { return m_strIp; }
    void SetIp(std::string val) { m_strIp = val; }
    uint16 GetPort() const { return m_nPort; }
    void SetPort(uint16 val) { m_nPort = val; }
private:
    void free_bufferevent();

private:
    CReconnSession*         m_pOwner = nullptr;
    bufferevent*            m_pBuffer = nullptr;
    CEventThread*           m_pThread = nullptr;
    CThreadDispatcher*      m_pDispatcher = nullptr;
    std::string             m_strIp;
    uint16                  m_nPort = 0;
    bool                    m_bConnected = false;
    bool                    m_bConnecting = false;
    uint32                  m_nConnectNum = 0;
    uint32                  m_nConnectAll = 0;
    uint32                  m_maxBuffer = 0;
    uint32                  m_trigger = 0;
    GenTimer                m_tmrConnect;
    bool                    m_bTimerStop = false;
    sockaddr_in             m_addr;
};

