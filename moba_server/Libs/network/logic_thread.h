#pragma once
#include <string>
#include "net_define.h"
#include "event_thread.h"
#include "listen_thread.h"
#include "thread_dispatcher.h"
#include "time_event.h"

class CThreadDispatcher;
class CReconnEvent;
class CReconnSession;
class CLogicInterface;
struct tagNetMsg;

class CLogicThread
{
protected:
    CLogicThread();
    ~CLogicThread();

public:
    static CLogicThread* newm(net_setting& setting);
    static void del(CLogicThread* pThread);

    bool InitLogic();
    CEventThread* evthread();

    int run();
    int stop();

    int add_reconn(CReconnSession* pSession,
        const std::string& strIp, uint16_t nPort, int iSecs, uint32_t maxBuffer);
    int remove_reconn(CReconnSession* pSession);

    bool send_listen(tagNetMsg* pNetMsg);
    bool send_reconn(tagNetMsg* pNetMsg);

    void reg_interface_listen(CLogicInterface* pLogicIF);
    void reg_interface_reconn(CLogicInterface* pLogicIF);

    void trigger(uint32_t dwTM);
    CLogicInterface* GetInterFace();
    CLogicInterface* GetReConnInterFace();
    CThreadDispatcher* Dispatcher();
    CThreadDispatcher* ReDispatcher();

    net_setting GetSetting() const { return m_setting; }
    void SetSetting(net_setting& val) { m_setting = val; }

private:
    bool create_listener(CThreadDispatcher* pDispatcher, CLogicThread* pThread, net_setting& conf);
    bool create_dispatcher(CThreadDispatcher* pDispatcher, CLogicThread* pThread,
        io_thread_setting& conf);

    CEventThread*           m_pEvThread = nullptr;
    CThreadDispatcher*      m_pDispatcher = nullptr;
    CListenThread*          m_pListenThread = nullptr;
    CThreadDispatcher*      m_pReConnDispatcher = nullptr;
    CTimeEvent*             m_pTimercon = nullptr;
    net_setting             m_setting;
};
