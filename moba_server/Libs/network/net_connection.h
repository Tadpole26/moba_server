#pragma once
#include <event2/event.h>
#include "net_define.h"
#include "time_event.h"
#include "id_alloctor.h"
#include <map>
#include <set>

class CNetConn;
class CNetConnPool;
class CIoThread;
struct bufferevent;
//对bufferevent封装
class CNetConn
{
public:
    CNetConn(conn_oid_t iConnId, CNetConnPool* pConnPool);
    ~CNetConn();

    int init(evutil_socket_t fd, event_base* pEvBase, sockaddr& addr, EConn_Type type
        , uint32_t nSecRecvLimit);
    int release();

    bufferevent* get_ev_buffer() { return m_pBuffer; }
    conn_oid_t get_conn_id() { return m_iConnId; }
    thread_oid_t get_thread_id();
    CNetConnPool* get_pool() { return m_pConnPool; }
    CIoThread* get_thread();
    uint32_t get_ip() { return m_ip; }
    EConn_Type get_type() { return m_type; }
    bool isvaid() { return m_type != ct_unknown; }

    uint32_t get_max_buffer() { return m_nMaxBufferSize; }
    void set_max_buffer(uint32_t nMaxBuffer) { m_nMaxBufferSize = nMaxBuffer; }

    bool recv_sec();
    time_t get_last_recv() { return m_tmLast; }
    uint32_t get_sec_recv() { return m_nSecRecvs; }
    uint32_t get_all_recv() { return m_nAllRecv; }

    EParse_Msg_Relt GetParseCode() const { return m_eParseCode; }
    void SetParseCode(EParse_Msg_Relt val) { m_eParseCode = val; }

    int write(const void* pData, size_t size);
private:
    conn_oid_t m_iConnId = invalid_conn_oid;
    EConn_Type m_type = ct_unknown;

    CNetConnPool* m_pConnPool = nullptr;
    bufferevent* m_pBuffer = nullptr;
    uint32_t m_nMaxBufferSize = DEF_MAX_BUFFER;

    sockaddr m_addr;
    uint32_t m_ip = 0;
    time_t m_tmLast = 0;
    uint32_t m_nSecRecvs = 0;
    uint32_t m_nSecRecvLimit = 0;
    uint32_t m_nAllRecv = 0;
    evutil_socket_t m_socket = -1;
    EParse_Msg_Relt m_eParseCode = pmr_ok;

    static void conn_readcb(bufferevent* pBuffer, void* args);
    static void conn_writecb(bufferevent* pBuffer, void* args) {}
    static void conn_eventcb(bufferevent* pBuffer, short events, void* args);
};

//管理所有的bufferevent
class CNetConnPool
{
public:
    CNetConnPool();
    ~CNetConnPool();

    bool init(CIoThread* pThread, uint16_t nMaxConn);

    int release();
    CNetConn* findconn(conn_oid_t id);
    CNetConn* newconn(uint32_t maxBuffer);

    int delconn(conn_oid_t id);
    int killconn(conn_oid_t);

    int write(conn_oid_t id, const void* pData, size_t size);
    int disconn(conn_oid_t id);

    bool is_valid(conn_oid_t id) { return id >= 0 && id < _nMaxConns; }
    bool start_timer(int nDetectSec);

    CIoThread* iothread() { return _pIoThread; }
    int get_ip(conn_oid_t id);

    size_t conn_size();
    bool Full();
    void remove_killed();

protected:
    CNetConn** _pConns = nullptr;
    conn_oid_t _nMaxConns = 5000;
    CIoThread* _pIoThread = nullptr;

    CIdAlloctor<uint16_t, UINT16_MAX> _IdAlloctor;
    std::set<uint16_t> _setID;

    CTimeEvent _tvdetect;
    void OnDetect(uint32_t dwTM);
    void net_conn_detect(uint32_t dwTM);
    void net_conn_kill(uint32_t dwTM);

    int m_nDetectSec = CHECK_DETECT_INTERVAL;
    std::map<conn_oid_t, time_t> _mapkill;
};