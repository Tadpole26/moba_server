#pragma once
//zookeeperÊÂ¼þ
#include <event2/event.h>
#include <event2/event_struct.h>
#include <functional>
#include "zookeeper.h"
#include "net_define.h"

typedef std::function<void()> fn_Updater;
class CZkEvent
{
public:
    CZkEvent();
    CZkEvent(event_base* pEvBase, bool bPersist, zhandle_t* pZkHandle, uint32_t iMilliSecs, fn_Updater func);
    virtual ~CZkEvent();
    virtual void trigger(evutil_socket_t fd, short event);

    bool init(event_base* pEvBase, bool bPersist, zhandle_t* pZkHandle, uint32_t iMilliSecs, fn_Updater func);

    bool init();
    void stop();
    uint32_t millisecs();

private:
    bool createtm();
    event _ev;
    bool _persist = true;
    struct timeval _tval;
    uint32_t _millisecs = 0;
    fn_Updater _func = nullptr;
    bool _brun = false;
    event_base* _pEvbase = nullptr;
    zhandle_t* _pZkHandle = nullptr;            //zookeeperÁ´½Óhandle
};


