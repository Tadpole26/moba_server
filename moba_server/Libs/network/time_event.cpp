#include "pch.h"
#include "time_event.h"
#include "listen_thread.h"
#include "event_thread.h"
#include "log_mgr.h"
static void timeout_cb(evutil_socket_t fd, short event, void* arg)
{
    CTimeEvent* pTimeEvent = (CTimeEvent*)arg;
    pTimeEvent->trigger();
}

CTimeEvent::CTimeEvent(event_base* pEvBase, bool bPersist, uint32_t iMilliSecs, fn_Timer func)
    : _persist(bPersist)
    , _millisecs(iMilliSecs)
    , _func(func)
{
    _pEvbase = pEvBase;
    createtm();
}

CTimeEvent::CTimeEvent()
{
}

CTimeEvent::~CTimeEvent()
{
    stop();
}

void CTimeEvent::trigger()
{
    if (_func != nullptr)
        _func(_millisecs);
}


bool CTimeEvent::init(event_base* pEvBase, bool bPersist, uint32_t iMilliSecs, fn_Timer func)
{
    stop();

    _pEvbase = pEvBase;
    _persist = bPersist;
    _millisecs = iMilliSecs;
    _func = func;

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

bool CTimeEvent::init()
{
    if (event_add(&_ev, &_tval) == 0)
    {
        _brun = true;
        return true;
    }
    return false;
}

void CTimeEvent::stop()
{
    if (_brun)
    {
        event_del(&_ev);
        _brun = false;
    }
}

uint32 CTimeEvent::millisecs()
{
    return _millisecs;
}

bool CTimeEvent::createtm()
{
    short nFlags = 0;
    if (_persist)
        nFlags |= EV_PERSIST;

    _tval.tv_sec = _millisecs / 1000;
    _tval.tv_usec = (_millisecs % 1000) * 1000;
    int r = event_assign(&_ev, _pEvbase, -1, nFlags, timeout_cb, (void*)this);
    return (r == 0);
}













