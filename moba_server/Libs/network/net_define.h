#pragma once

#include <stdio.h>
#include <string.h>
#include <string>

#ifndef WIN32
#define SP_THREAD_CALL
#else 
#define SP_THREAD_CALL __stdcall
#endif 


using conn_oid_t = uint16_t;
const conn_oid_t invalid_conn_oid = 0xffff;

using thread_oid_t = uint16_t;
const thread_oid_t invalid_thread_oid = 0xff;

using session_oid_t = uint32_t;
const session_oid_t invalid_session_oid = 0xffffffff;

using host_msg_t = int8_t;
using msg_id_t = uint8_t;

const int32_t DEF_MAX_BUFFER = 4 * 1024 * 1024;

const int32_t CHECK_DETECT_INTERVAL = 10; // 10S

#ifndef WIN32
#include <netinet/in.h>
#ifdef _XOPEN_SOURCE_EXTENDED
#include <arpa/inet.h>
#endif 
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#endif

enum E_Mq_Type
{
    EMQ_CAS = 0, //单进单出
    EMQ_MUL = 1, //多进单出
    EMQ_GEN = 2, //多线程
    EMQ_NO = 3, //网络层单线程处理
};

enum EParse_Msg_Relt
{
    pmr_ok = 0,
    pmr_invalid_size = 1,
    pmr_buffer_error = 2,
    pmr_verify_failed = 3,
    pmr_push_failed = 4,
    pmr_more_failed = 5,
    pmr_cus_kill = 6
};

enum EConn_State
{
    cs_unknown = 0,
    cs_net_accepted = 1,
    cs_net_connected = 2,
    cs_net_disconn = 3,
    cs_layer_disconn = 4,
    cs_layer_connected = 5,
    cs_net_norecv = 6,
    cs_layer_kill = 7,
    cs_net_exit = 8,   //进程退出
};

enum EConn_Type
{
    ct_unknown = 0,
    ct_accept = 1,
    ct_connect = 2,
};

struct io_thread_setting
{
    thread_oid_t    m_nIos = 0;
    conn_oid_t      m_ConnsOfIo = 0;

    size_t          m_acceptMaxSize = 64 * 1024;
    size_t          m_inMaxSize = 256 * 1024;
    size_t          m_outMaxSize = 256 * 1024;
    int             m_nDetectSec = CHECK_DETECT_INTERVAL * 12;
    int             m_nSecRecvLimit = 1024 * 1024;

    //消息队列的方式
    E_Mq_Type       m_syncType = EMQ_MUL;

    E_Mq_Type MqType() const { return m_syncType; }

    bool IsValid()
    {
        return m_nIos > 0 && m_ConnsOfIo > 0;
    }

    void Init(thread_oid_t nIos, conn_oid_t ConnsOfIo
        , size_t acceptMaxSize, size_t inMaxSize
        , size_t outMaxSize, int nSecRecvLimit = 1024 * 1024
        , int nDetectSec = CHECK_DETECT_INTERVAL * 12)
    {
        m_nIos = nIos;
        m_ConnsOfIo = ConnsOfIo;
        m_acceptMaxSize = acceptMaxSize;
        m_inMaxSize = inMaxSize;
        m_outMaxSize = outMaxSize;

        if (nDetectSec < CHECK_DETECT_INTERVAL * 2)
            m_nDetectSec = CHECK_DETECT_INTERVAL * 2;
        else
            m_nDetectSec = nDetectSec;

        m_nSecRecvLimit = nSecRecvLimit;
    }
};


struct net_setting
{
    std::string m_strListenIp = "0.0.0.0";
    uint16_t m_nListenPort = 0;

    //监听IO
    io_thread_setting m_ioThread;

    //连接IO
    io_thread_setting m_reThread;

    //消息队列的方式
    E_Mq_Type       m_syncType = EMQ_MUL;

    uint32_t          m_ReconSec = 1;
    void InitMq(E_Mq_Type eType)
    {
        m_syncType = eType;
        m_ioThread.m_syncType = eType;
        m_reThread.m_syncType = eType;

        if (m_syncType == EMQ_NO)
        {
            m_ioThread.m_nIos = 1;
            m_reThread.m_nIos = 1;
        }
    }

    E_Mq_Type MqType() const { return m_syncType; }
};

