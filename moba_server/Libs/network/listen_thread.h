#pragma once

#include <vector>
#include <thread>

#include "net_define.h"
#include "event_thread.h"
#include "msg_queue.h"

struct evconnlistener;
class CThreadDispatcher;
class CListenThread
{
public:
    CListenThread();
    ~CListenThread();

    int Init(CEventThread* pLogicThread, net_setting& conf, CThreadDispatcher* pDispatcher);
    int Start();

    int Fini();
    int Stop();

    CEventThread* EvThread() { return m_pEvThread; }
    CThreadDispatcher* Dispatcher() { return m_pDispatcher; }
    CMsgQueue& OutPut();

private:
    int AddListen(const char* szIp, uint16_t port);
    void msg_out_fn(const event_msg_t msg);

    static unsigned SP_THREAD_CALL listen_thread_fn(void* args);
    static void listen_callback(struct evconnlistener* pListenner, evutil_socket_t fd,
        struct sockaddr* pSockAddr, int socklen, void* user_data);

    CEventThread*           m_pEvThread = nullptr;      //开启线程,进行事件循环
    evconnlistener*         m_pListener = nullptr;      
    uint16_t                 m_port = 0;                 //监听端口号
    CThreadDispatcher*      m_pDispatcher = nullptr;
    CMsgQueue               m_OutputQueue;
    bool                    m_bStop = false;            //监听线程是否停止
};
