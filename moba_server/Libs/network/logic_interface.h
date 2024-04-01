#pragma once
#include <vector>
#include "net_define.h"
#include "msg_interface.h"
#include <list>
#include <string>
#include <map>

class CThreadDispatcher;
enum ESession_State
{
    ss_unknown = 0,
    ss_conn_reg_ok = 2,
    ss_conn_reg_failed = 3,
    ss_conn_disconn = 4,
};

//每个连接信息
class CLogicThread;
class CLogicSession
{
public:
    CLogicSession();
    virtual ~CLogicSession();

    virtual void on_accept() {}
    virtual void on_connect() {}
    virtual void on_disconnect() {}
    virtual void on_prepared() {}
    virtual void handle_msgv(const void* pMsg) = 0;

    virtual void accpeted();
    virtual void connected();
    virtual void disconnected();
    void set_dispatcher(CThreadDispatcher* dispatcher) { m_pDispatcher = dispatcher; }
    CThreadDispatcher* get_dispatcher() { return m_pDispatcher; }
    time_t get_dicon_time() { return m_tmDiscon; }

    bool IsTimeOut() const { return m_bTimeOut; }
    void SetTimeOut(bool val) { m_bTimeOut = val; }
public:
    bool is_connected();

    void set_hd(const tagHostHd& hd) { m_hd = hd; }
    const tagHostHd& get_hd() const { return m_hd; }

    void set_soid(session_oid_t id) { m_soid = id; }
    session_oid_t get_soid() const { return m_soid; }

    EConn_Type type() { return m_type; }

    thread_oid_t get_toid() const { return m_hd.m_threadOid; }
    conn_oid_t get_coid() const { return m_hd.m_connOid; }

    void set_ip(uint32_t ip);
    uint32_t get_ip() const { return m_ip; }
    std::string get_ip_str() { return m_strip; }

    bool has_valid_hd() const
    {
        return m_hd.m_threadOid != invalid_thread_oid && m_hd.m_connOid != invalid_conn_oid;
    }
protected:
    tagHostHd m_hd;
    CThreadDispatcher* m_pDispatcher = nullptr;

    session_oid_t       m_soid = 0;
    ESession_State      m_state = ss_unknown;
    uint32_t             m_ip = 0;
    std::string         m_strip;
    time_t              m_tmDiscon = 0;
    EConn_Type          m_type = ct_accept;
    bool                m_bTimeOut = false;
};


class CReconnEvent;
class CReconnSession : public CLogicSession
{
public:
    CReconnSession();
    virtual ~CReconnSession();

    virtual void set_reconn_event(CReconnEvent* pReconn);
    CReconnEvent* get_reconn_event() const { return m_pReconn; }

    virtual void disconnected();
    virtual void trigger() = 0;
    virtual void connecttimes(uint32_t times) { }

    void stop();

    std::string GetReconIp();
    uint16_t GetReconPort();
private:
    CReconnEvent* m_pReconn = nullptr;
};

/*
*  管理所有连接,dispatcher有该类,管理所有线程连接的用户信息
*/
class CLogicInterface
{
public:
    CLogicInterface();
    virtual ~CLogicInterface();

    virtual CLogicSession* new_server_session();

    virtual void handle_logic_msg(const tagNetMsg* pNetMsg);

    virtual void handle_logic_msgv(const void* pMsg);

    void stop();

public:
    void initialize(CThreadDispatcher* pDispatcher, thread_oid_t tNum, conn_oid_t conNum);
    void handle_conn_msg(const tagConnMsg* pConnMsg);
    int add_session(CLogicSession* pSession, const tagHostHd& hd, uint32_t maxBuffer);
    int kill_session(CLogicSession* pSession);
    CLogicSession* get_session(const tagHostHd& hd);

    session_oid_t add_pre_session(CReconnSession* pSession);
    CReconnSession* get_pre_session(session_oid_t id);
    int kill_pre_session(CReconnSession* pSession);

    int send_io_conn_msg(const tagHostHd& hd, EConn_State state, session_oid_t soid, uint32_t maxBuffer);
    std::map<session_oid_t, CReconnSession*>& get_pre_sessions() { return m_PreSession; }

protected:
    bool has_session(const tagHostHd& hd);
    int remove_session(CLogicSession* pSession);

    CThreadDispatcher* m_pDispatcher = nullptr;
    typedef std::vector<CLogicSession*> session_vec;
    typedef std::vector<session_vec> session_vecs;      //vector[线程数量][每个线程连接数量]
    session_vecs m_sessionVecs;

    //connect session
    static session_oid_t m_nPreNum;
    std::map<session_oid_t, CReconnSession*> m_PreSession;
};