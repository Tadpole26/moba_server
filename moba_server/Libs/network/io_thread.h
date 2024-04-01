#pragma once

#include <vector>
#include <thread>

#include "net_define.h"
#include "event_thread.h"
#include "msg_queue.h"
#include "net_connection.h"

class CThreadDispatcher;
class CNetConnPool;
class CThreadDispatcher;
struct tagAcceptConnMsg;
struct tagConnMsg;
class CIoThread
{
public:
    CIoThread();
    ~CIoThread();

    int Init(thread_oid_t iOid, const io_thread_setting& conf,
        CThreadDispatcher* pDispatcher, CEventThread* pLogicThread);
    void Fini();
    void Stop();

    CEventThread* EvThread();

    CMsgQueue& InPut();
    CMsgQueue& OutPut();
    CMsgQueue& AcceptPut();

    CNetConnPool* ConnPool();
    CThreadDispatcher* GetParent();

    void AddPut() { ++m_put_out_count; }
    void AddGet() { ++m_get_out_count; }

private:
    CEventThread* m_pEvThread = nullptr;

    uint32_t m_put_out_count = 0;
    uint32_t m_get_out_count = 0;

    CNetConnPool        m_connPool;
    CThreadDispatcher* m_pParent = nullptr;
    bool                m_bStop = false;

    CMsgQueue m_InputQueue;
    CMsgQueue m_OutputQueue;
    CMsgQueue m_AcceptConnQueue;
    io_thread_setting m_setting;

    void msg_accept_fn(const event_msg_t msg);
    void msg_input_fn(const event_msg_t msg);
    void msg_output_fn(const event_msg_t msg);

    void io_handle_conn_msg(tagConnMsg* pConnMsg);
};