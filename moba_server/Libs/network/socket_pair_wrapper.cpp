#include "pch.h"
#include "socket_pair_wrapper.h"
#include <iostream>

int CSocketPairWrapper::write()
{
    if (!_bInit) return 0;

    int rc = 0;
#ifdef WIN32
    rc = ::send(_fdWrite, "", 1, 0);
#else 
    rc = ::send(_fdWrite, "", 1, MSG_DONTWAIT);
#endif 

    if (rc < 0 || rc == EAGAIN || rc == EINTR)
        std::cout << "error!" << std::endl;
    return rc;
}

void CSocketPairWrapper::read()
{
    if (!_bInit) return;

    static unsigned char buf[1024];

#ifdef WIN32 
    while (::recv(_fdRead, (char*)buf, sizeof(buf), 0) >= 1024) {}
#else 
    while (::recv(_fdRead, (char*)buf, sizeof(buf), MSG_DONTWAIT) >= 1024) {}
#endif
}

bool CSocketPairWrapper::init(event_base* pEvBase, Fn_Pair_Call fn, void* args)
{
    evutil_socket_t fds[2];
    if (evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, fds) == -1)
        return false;

    _fdWrite = fds[0];
    _fdRead = fds[1];
    _pEvBase = pEvBase;

    if (!CNetOpt::SetNoBlock(_fdWrite) || !CNetOpt::SetNoBlock(_fdRead))
    {
        std::cout << "setsockopt noblock client:" << _fdRead << ", " << _fdWrite << ", err:" << errno << std::endl;
        return false;
    }

    event_assign(&_ev, pEvBase, _fdRead, EV_READ | EV_PERSIST, event_pop, this);
    event_add(&_ev, NULL);

#ifdef _WIN32 
    if (!CNetOpt::SetNoDelay(_fdWrite))
    {
        std::cout << "setsockopt nodelay client:" << _fdWrite << ", err:" << errno << std::endl;
        return false;
    }

    if (!CNetOpt::SetNoDelay(_fdRead))
    {
        std::cout << "setsockopt nodelay client:" << _fdRead << ", err:" << errno << std::endl;
        return false;
    }
#else 
#endif   // _WIN32

    _bInit = true;
    _fnCall = fn;
    _args = args;

    return true;
}

bool CSocketPairWrapper::release()
{
    if (!_bInit) return true;

    if (_pEvBase == nullptr) return true;

    event_del(&_ev);
    if (_fdRead != -1)
    {
        evutil_closesocket(_fdRead);
        _fdRead = -1;
    }
    if (_fdWrite != -1)
    {
        evutil_closesocket(_fdWrite);
        _fdWrite = -1;
    }

    _pEvBase = nullptr;
    _bInit = false;

    return true;
}

void CSocketPairWrapper::stop()
{
    _bStop = true;
    write();
}

bool CSocketPairWrapper::isstop()
{
    return _bStop;
}

void CSocketPairWrapper::event_pop(evutil_socket_t fd, short events, void* args)
{
    CSocketPairWrapper* pMsgQueue = (CSocketPairWrapper*)args;
    if (pMsgQueue != nullptr)
    {
        pMsgQueue->read();
        pMsgQueue->docall();
        if (pMsgQueue->isstop())
            pMsgQueue->release();
    }
}

void CSocketPairWrapper::docall()
{
    if (_fnCall != nullptr && _args != nullptr)
        _fnCall(_args);
}
