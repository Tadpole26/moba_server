#pragma once

#include "msg_interface.h"
#include "net_define.h"

struct bufferevent;
class CNetConn;
class CMsgQueue;
class CLogicThread;

#pragma pack(push)
#pragma pack(1)

#define GAME_BASE_FLAG 0x8000
#define GAME_ZIP_FLAG 0x8100
#define GAME_UDP_FLAG 0x8001
#define GAME_UDP_PING_FLAG 0x8002

struct tagMsgHead
{
    uint32_t    uiLen = 0;                     //uint32_t wLen;
    uint16_t    uiFlag = GAME_BASE_FLAG;       //什么类型的包
    uint8_t     usModuleId = 0;                //主类型  uint8 is ok
    uint32_t    uiCmdId = 0;                   //次类型
    uint32_t    uiCode = 0;                    //code值,错误码
    uint32_t    uiSeqid = 0;                   //Sequence Id
    uint32_t    uiCrc = 0;                     //校验
};

#define MSG_MAX_LEN 64000
#define NET_HEAD_SIZE sizeof(tagMsgHead)
#define NET_HEAD_MSG(buf) (tagMsgHead*)(buf)
#define NET_DATA_BUF(head) (uchar*)(head) + NET_HEAD_SIZE
#define NET_DATA_SIZE(head) ((head)->uiLen) - NET_HEAD_SIZE

#pragma pack( pop )

//发送消息
extern bool my_send_conn_msg(CThreadDispatcher* pDispatcher, thread_oid_t toid
    , conn_oid_t coid, const tagMsgHead* pMsg);

//发送到多个连接上, 广播消息
extern bool my_multicast_conn_msg(CThreadDispatcher* pDispatcher, thread_oid_t toid
    , const conn_oid_t* pCoids, uint16_t count, const tagMsgHead* pMsg);

extern EParse_Msg_Relt my_io_recv_msg(CNetConn* pConn, CMsgQueue* pInputQueue);
extern void my_io_send_msg(const void* pMsg, void* args);

extern tagMsgHead* make_head_msg(const char* pBuf, size_t size_t
    , char* arrBuffer, uint8_t usModule, uint32_t uiCmd
    , uint32_t uiSequence, uint32_t eCode);
