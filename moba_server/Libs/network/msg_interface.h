#pragma once
#include "net_define.h"
//#include "../platform/mellocx.h"
#include <event2/util.h>

enum E_Host_Mt_Type
{
    HMT_CONN = 0,
    HMT_NET = 1,
    HMT_MUL_NET = 2,
};

#pragma pack(push)
#pragma pack(1)

struct tagHostHd
{
    thread_oid_t m_threadOid = 0;
    conn_oid_t m_connOid = 0;
    host_msg_t m_type = 0;
};

struct tagConnMsgBody
{
    EConn_State m_state = cs_unknown;
    session_oid_t m_soid = 0;
    uint32_t m_maxBuffer = 0;
};

struct tagAcceptConnMsg
{
    evutil_socket_t m_fd = -1;
    EConn_Type m_connType = ct_accept;
    session_oid_t m_soid = 0;

    uint32_t m_maxBuffer = 0;
    sockaddr m_addr;
};

struct tagConnMsg
{
    tagHostHd m_hd;
    tagConnMsgBody m_body;
};

struct tagNetMsg
{
    tagHostHd m_hd;
    uint8_t m_body[1];
};

struct tagBufItem
{
    size_t  _size = 0;
    uint8_t _body[1];
};

#define NET_CONN_SIZE sizeof(tagConnMsg)
#define NET_HOST_SIZE sizeof(tagNetMsg)
#define NET_ACCEPT_SIZE sizeof(tagAcceptConnMsg)
#define BUF_ITEM_SIZE sizeof(tagBufItem)

#pragma pack( pop )

struct bufferevent;
class CNetConn;
class CMsgQueue;
class CListenThread;

typedef EParse_Msg_Relt(*fn_io_recv_msg)(CNetConn* pConn, CMsgQueue* pInputQueue);
typedef void (*fn_io_send_msg)(const void* pMsg, void* args);

void regfn_io_recv_msg(fn_io_recv_msg fn);
void regfn_io_send_msg(fn_io_send_msg fn);

class CThreadDispatcher;

int io_notify_logic_conn_msg(CNetConn* pConn, EConn_State state, session_oid_t soid);
int logic_notify_io_conn_msg(CThreadDispatcher* pDispatcher, const tagHostHd& hd
    , EConn_State state, session_oid_t soid, uint32_t maxBuffer);

int logic_notify_io_conn_msg(CThreadDispatcher* pDispatcher, EConn_State state);
int logic_notify_listen_conn_msg(CListenThread* pListen, EConn_State state);

tagConnMsg* conn_msg_alloc();
tagAcceptConnMsg* acc_conn_alloc();
tagNetMsg* net_msg_alloc(uint32_t nsize);
tagBufItem* buf_item_alloc(uint32_t nsize);
