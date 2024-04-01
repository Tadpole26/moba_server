
#include "pch.h"
#include "net_opt.h"
#ifndef _WIN32
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <unistd.h>
#endif 
#include <event2/listener.h>
#include <event2/event.h>
#include "log_mgr.h"

CNetOpt::CNetOpt() {}

CNetOpt::~CNetOpt() {}

bool CNetOpt::SetKeepALive(evutil_socket_t fd)
{
    const int32_t keepAlive = 1;        // 设定KeepAlive(底层定时心跳)
    const int32_t keepIdle = 60;        // 开始首次KeepAlive探测前的TCP空闲时间
    const int32_t keepInterval = 5;     //两次KeepAlive探测间的时间间隔
    const int32_t keepCount = 10;       //判定断开前的KeepAlive探测次数
    const int32_t nodelay = 1;          //tcp nodelay(禁用nodelay算法)

    //设置socket选项开启keep alive
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepAlive, sizeof(keepAlive)) == -1)
        Log_Error("setsocketopt SO_KEEPALIVE error!");

#ifndef _WIN32
    if (setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, (char*)&keepIdle, sizeof(keepIdle)) == -1)
        Log_Error("setsockopt TCP_KEEPIDLE error!");

    if (setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, (char*)&keepInterval, sizeof(keepInterval)) == -1)
        Log_Error("setsockopt TCP_KEEPINTVL error!");

    if (setsockopt(fd, SOL_TCP, TCP_KEEPCNT, (char*)&keepCount, sizeof(keepCount)) == -1)
        Log_Error("setsockopt TCP_KEEPCNT error!");
#endif 
    //设置socket选项禁用nodelay算法
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(nodelay)) == -1)
    {
        Log_Error("setsockopt client:%d, err:%d", fd, errno);
        return false;
    }
    return true;
}

//设置socket选项设置缓冲区大小
bool CNetOpt::SetRcvSize(evutil_socket_t fd, int32_t size)
{
    int32_t rcvbuf_len;
#ifdef _WIN32
    int32_t len = sizeof(rcvbuf_len);
#else 
    uint32_t len = sizeof(rcvbuf_len);
#endif //_WIN32

    if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&rcvbuf_len, &len) < 0)
    {
        Log_Error("getsockopt: ");
        return false;
    }
    if (rcvbuf_len >= size) return true;
    rcvbuf_len = size;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (char*)&rcvbuf_len, len) < 0)
    {
        Log_Error("setsockopt: ");
        return false;
    }
    return true;
}

//设置socket选项设置不进行缓冲区拷贝
bool CNetOpt::SetSndSize(evutil_socket_t fd, int32_t size)
{
    int32_t rcvbuf_len;

#ifdef _WIN32
    int32_t len = sizeof(rcvbuf_len);
#else 
    uint32_t len = sizeof(rcvbuf_len);
#endif //_WIN32
    if (getsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&rcvbuf_len, &len) < 0)
    {
        Log_Error("getsockopt: ");
        return false;
    }

    if (rcvbuf_len >= size) return true;
    rcvbuf_len = size;
    if (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (char*)&rcvbuf_len, len) < 0)
    {
        Log_Error("setsockopt: ");
        return false;
    }
    return true;
}

bool CNetOpt::SetBufSize(evutil_socket_t fd, int32_t size /*= 65536*/)
{
    SetRcvSize(fd, size);
    SetSndSize(fd, size);
    return true;
}

bool CNetOpt::SetCloseExec(evutil_socket_t fd)
{
#ifndef _WIN32
    int flags = fcntl(fd, F_GETFD);
    if (flags < 0)
    {
        Log_Error("SetColseExec");
        return false;
    }

    flags |= FD_CLOEXEC;
    if (fcntl(fd, F_SETFD, flags) < 0)
    {
        Log_Error("SetColseExec");
        return false;
    }
#endif   // _WIN32
    return true;
}

//设置socket选项设置禁用nodelay算法
bool CNetOpt::SetNoDelay(evutil_socket_t fd)
{
    const int32_t nodelay = 1;

    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(nodelay)) == -1)
    {
        Log_Error(" setsockopt client:%d, err:%d", fd, errno);
        return false;
    }
    return true;
}

//设置socket选项设置设置套接字非阻塞(直接抛出errno)
bool CNetOpt::SetNoBlock(evutil_socket_t fd)
{
#ifdef WIN32
    {
        u_long nonblocking = 1;
        if (ioctlsocket(fd, FIONBIO, &nonblocking) == SOCKET_ERROR)
            return false;
    }
#else 
    {
        int flags;
        if ((flags = fcntl(fd, F_GETFL, NULL)) < 0)
            return false;
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
            return false;
    }
#endif 
    return true;
}

bool CNetOpt::SetSockReuseable(evutil_socket_t fd)
{
#ifndef WIN32
    int one = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void*)&one, (ev_socklen_t)sizeof(one)) == 0)
        return true;
    else
        return false;
#else 
    return true;
#endif
}

int inet_ntop4(const unsigned char* src, char* dst, size_t size)
{
    static const char fmt[] = "%u.%u.%u.%u";
    char tmp[16];
    int l;

    l = snprintf(tmp, sizeof(tmp), fmt, src[0], src[1], src[2], src[3]);
    if (l <= 0 || (size_t)l > size)
        return -1;

    strncpy(dst, tmp, size);
    dst[size - l] = '\0';
    return 0;
}

std::string CNetOpt::Inet_Ip(sockaddr& addr)
{
    uint32 ip = ((sockaddr_in*)(&addr))->sin_addr.s_addr;
    return Inet_Ip(ip);
}

std::string CNetOpt::Inet_Ip(uint32 ip)
{
    char chIp[16] = { 0 };
    struct in_addr in;
    memcpy(&in, &ip, sizeof(ip));

    if (inet_ntop4((const unsigned char*)&in, chIp, 16) != 0)
        return "0.0.0.0";

    return (std::string)(chIp);
}

std::string CNetOpt::GetLocalIpAddress()
{
    char local[255] = { 0 };
    gethostname(local, sizeof(local));
    hostent* ph = gethostbyname(local);
    if (ph == nullptr)
        return "";

    in_addr addr;
    memcpy(&addr, ph->h_addr_list[0], sizeof(in_addr)); //这里仅获取第一个ip
    std::string localIP;
    localIP.assign(inet_ntoa(addr));
    return localIP;
}