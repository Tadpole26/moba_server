#pragma once
//socket —°œÓ…Ë÷√

#include "event2/util.h"
#include <string>

class CNetOpt
{
protected:
    CNetOpt();
    ~CNetOpt();
public:
    static bool SetKeepALive(evutil_socket_t fd);
    static bool SetRcvSize(evutil_socket_t fd, int32_t size = 65536);
    static bool SetSndSize(evutil_socket_t fd, int32_t size = 65536);

    static bool SetBufSize(evutil_socket_t fd, int32_t size = 65536);
    static bool SetCloseExec(evutil_socket_t fd);
    static bool SetNoDelay(evutil_socket_t fd);
    static bool SetNoBlock(evutil_socket_t fd);
    static bool SetSockReuseable(evutil_socket_t fd);

    static std::string Inet_Ip(sockaddr& addr);
    static std::string Inet_Ip(uint32_t ip);
    static std::string GetLocalIpAddress();
};
