#include "pch.h"
#include "zk_event.h"
#include "listen_thread.h"
#include "event_thread.h"
#include "log_mgr.h"

//zk关注数据变化回调
static void zkEvent_cb(evutil_socket_t fd, short event, void* arg)
{
    CZkEvent* pZkEvent = (CZkEvent*)arg;
    pZkEvent->trigger(fd, event);
}

CZkEvent::CZkEvent(event_base* pEvBase, 
    bool bPersist, 
    zhandle_t* pZkHandle, 
    uint32_t iMilliSecs,
    fn_Updater func)
    : _persist(bPersist)
    , _millisecs(iMilliSecs)
    , _func(func)
{
    _pEvbase = pEvBase;
    _pZkHandle = pZkHandle;
    createtm();
}

CZkEvent::CZkEvent()
{
}

CZkEvent::~CZkEvent()
{
    stop();
}

void CZkEvent::trigger(evutil_socket_t fd, short event)
{
    if (_func != nullptr)
        _func();

    //触发zk事件回调
    if (_pZkHandle != nullptr)
    {
        int iZkEvent = 0;
        if (event & EV_READ) iZkEvent |= ZOOKEEPER_READ;
        if (event & EV_WRITE) iZkEvent |= ZOOKEEPER_WRITE;
        //process触发对应watch和complete事件回调
        zookeeper_process(_pZkHandle, iZkEvent);
        //重新从zkServer拿到感兴趣事件(io事件放入epoll中)
#ifdef WIN32
        SOCKET iSocketFd = -1;
#else
        int iSocketFd = -1;
#endif 
        int iInterestEvent = 0;
        struct timeval stTimeout;
        int iRet = zookeeper_interest(_pZkHandle, &iSocketFd, &iInterestEvent, &stTimeout);
        if (iRet != ZOK)
        {
            Log_Error("zk interest error, errcode:%d|errmsg:%s", 
                iRet, 
                zerror(iRet));
            zookeeper_close(_pZkHandle);
            _pZkHandle = nullptr;
            return;
        }
        else
        {
            short iZkEvent = 0;
            if (iInterestEvent & ZOOKEEPER_READ) iZkEvent |= EV_READ;
            if (iInterestEvent & ZOOKEEPER_WRITE) iZkEvent |= EV_WRITE;
            event_del(&_ev);
            event_assign(&_ev, _pEvbase, iSocketFd, iZkEvent, zkEvent_cb, this);
            event_add(&_ev, &stTimeout);
            return;
        }
    };
}


bool CZkEvent::init(event_base* pEvBase, 
    bool bPersist, 
    zhandle_t* pZkHandle,
    uint32_t iMilliSecs,
    fn_Updater func)
{
    stop();

    _pEvbase = pEvBase;
    _persist = bPersist;
    _millisecs = iMilliSecs;
    _func = func;
    _pZkHandle = pZkHandle;

    if (!createtm())
        return false;
    auto ret = event_add(&_ev, &_tval);
    if (ret == 0)
    {
        _brun = true;
        return true;
    }
    return false;
}

bool CZkEvent::init()
{
    if (event_add(&_ev, &_tval) == 0)
    {
        _brun = true;
        return true;
    }
    return false;
}

void CZkEvent::stop()
{
    if (_brun)
    {
        event_del(&_ev);
        _brun = false;
    }
}

uint32_t CZkEvent::millisecs()
{
    return _millisecs;
}

bool CZkEvent::createtm()
{
    short nFlags = 0;
    if (_persist)
        nFlags |= EV_PERSIST;

    _tval.tv_sec = _millisecs / 1000;
    _tval.tv_usec = (_millisecs % 1000) * 1000;
    int r = event_assign(&_ev, _pEvbase, -1, nFlags, zkEvent_cb, (void*)this);
    return (r == 0);
}













