#pragma once
//对libevent socketpair 进行管理

#include <event2/util.h>
#include <event2/event.h>
#include <event2/event_struct.h>
#include "net_opt.h"

typedef void(*Fn_Pair_Call)(void*);

class CSocketPairWrapper
{
public:
    CSocketPairWrapper() {}
    ~CSocketPairWrapper() { release(); }

    int write();
    void read();
    bool init(event_base* pEvBase, Fn_Pair_Call fn, void* args);
    bool release();
    void stop();
    bool isstop();

private:
    event_base* _pEvBase = nullptr;
    evutil_socket_t _fdWrite = -1;
    evutil_socket_t _fdRead = -1;
    event _ev;
    bool _bInit = false;
    void* _args = nullptr;
    Fn_Pair_Call _fnCall = nullptr;
    bool _bStop = false;

    static void event_pop(evutil_socket_t fd, short events, void* args);
    void docall();
};

