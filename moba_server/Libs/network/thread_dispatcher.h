#pragma once
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "net_define.h"
#include <event2/util.h>

class CThreadDispatcher;
class CIoThread;
class CEventThread;
struct tagNetMsg;
struct tagAcceptConnMsg;
class CLogicInterface;

typedef thread_oid_t(*fn_dispatch_thread)(CThreadDispatcher* pDispatcher);

class CThreadDispatcher
{
public:
    CThreadDispatcher();
    ~CThreadDispatcher();

    int init(CEventThread* pLogicThread,
        io_thread_setting& threadconf);
    int start();
    int fini();
    int stop();

    int dispatch_accpet(evutil_socket_t fd, session_oid_t soid, sockaddr addr
        , uint32_t maxBuffer, EConn_Type accepted = ct_accept);

    thread_oid_t get_threads();
    conn_oid_t get_thread_max_conn();

    void reg_Logic(CLogicInterface* pLogicIF);
    CLogicInterface* get_Logic();

    CIoThread* get_io_thread(thread_oid_t toid);
    bool send_msg_hton(void* pNetMsg);

    bool is_valid_io_thread(thread_oid_t iThreadOid);
    void install_dispatch(fn_dispatch_thread fn);
    static thread_oid_t dispatch_default(CThreadDispatcher* pDispatcher);
    static thread_oid_t dispatch_min(CThreadDispatcher* pDispatcher);

    void add_init_count();

    CLogicInterface* GetInterFace() { return m_pLogicIF; }
private:
    std::mutex                      m_lockInit;
    std::condition_variable         m_condInit;
    CIoThread*                      m_pIoThreads = nullptr;
    thread_oid_t                    m_iThreadNum = 0;
    thread_oid_t                    m_iThreadPos = 0;
    fn_dispatch_thread              m_fnDispatch = nullptr;
    conn_oid_t                      m_nMaxConnOfThread = 0;
    CLogicInterface*                m_pLogicIF = nullptr;
    thread_oid_t                    m_iCurInitCount = 0;
};