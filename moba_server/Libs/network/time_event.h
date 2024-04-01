#pragma once
#include <event2/event.h>
#include <event2/event_struct.h>
#include <functional>
#include "net_define.h"

class CEventThread;
typedef std::function<void(uint32_t)> fn_Timer;
class CTimeEvent
{
public:
    CTimeEvent();
    CTimeEvent(event_base* pEvBase, bool bPersist, uint32_t iMilliSecs, fn_Timer func);
    virtual ~CTimeEvent();
    virtual void trigger();

    bool init(event_base* pEvBase, bool bPersist, uint32_t iMilliSecs, fn_Timer func);

    bool init();
    void stop();
    uint32_t millisecs();

private:
    bool createtm();
    event _ev;
    bool _persist = true;
    struct timeval _tval;
    uint32_t _millisecs = 0;
    fn_Timer _func = nullptr;
    bool _brun = false;
    event_base* _pEvbase = nullptr;
};
