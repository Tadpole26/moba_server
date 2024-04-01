#pragma once
#include "logic_interface.h"
#include "tcp_client.h"
#include "msg_make.h"
#include "result_code.pb.h"
#include "msg_module.pb.h"
#include "global_define.h"
/*
client结构
(对于GateServer来说是玩家)
(对于GameServer来说是GateServer/WorldServer等等)
*/


class svr_session : public CTcpSession, public CMsgMake
{
public:
	svr_session();
	~svr_session();

	virtual bool Send_Msg(google::protobuf::Message* pMsg, msg_id_t usProtocol
		, msg_id_t usModule = ProtoMsg::ServerCommon, ProtoMsg::ResultCode code = ProtoMsg::ResultCode::Code_Common_Success
		, uint32 seqid = 0);

	virtual bool Send_Msg(const std::string& strMsg, msg_id_t usProtocol
		, msg_id_t usModule = ProtoMsg::ServerCommon, ProtoMsg::ResultCode code = ProtoMsg::ResultCode::Code_Common_Success
		, uint32 seqid = 0);

	virtual bool Send_Msg(const uchar* buf, size_t size, msg_id_t usProtocol
		, msg_id_t usModule = ProtoMsg::ServerCommon, ProtoMsg::ResultCode code = ProtoMsg::ResultCode::Code_Common_Success
		, uint32 seqid = 0);

	virtual void Init();
	virtual void Release();
	virtual bool Send(const tagMsgHead* pMsg);

	uint32 GetServerId() const { return m_uiServerId; }
	void SetServerId(uint32 val) { m_uiServerId = val; }

	SERVER_KIND GetServerKind() const { return m_eServerKind; }
	void SetServerKind(SERVER_KIND val) { m_eServerKind = val; }

protected:
	virtual void handle_msgv(const void* pMsg);
	virtual void handle_msg(const tagMsgHead* pMsg) = 0;

	uint32					m_uiServerId = 0;
	SERVER_KIND				m_eServerKind = SERVER_KIND_NONE;
};

class svr_reconn : public CTcpReconn, public CMsgMake
{
public:
	svr_reconn(bool bCache = true, uint32 CacheSec = DEF_CACHE_SECOND);
	~svr_reconn();

	virtual bool Send_Msg(google::protobuf::Message* pMsg, msg_id_t usProtocol
		, msg_id_t usModule = ProtoMsg::ServerCommon, ProtoMsg::ResultCode code = ProtoMsg::ResultCode::Code_Common_Success
		, uint32 seqid = 0);

	virtual bool Send_Msg(const std::string& strMsg, msg_id_t usProtocol
		, msg_id_t usModule = ProtoMsg::ServerCommon, ProtoMsg::ResultCode code = ProtoMsg::ResultCode::Code_Common_Success
		, uint32 seqid = 0);

	virtual bool Send_Msg(const uchar* buf, size_t size, msg_id_t usProtocol
		, msg_id_t usModule = ProtoMsg::ServerCommon, ProtoMsg::ResultCode code = ProtoMsg::ResultCode::Code_Common_Success
		, uint32 seqid = 0);

	virtual bool Send(const tagMsgHead* pMsg);

	uint32 GetServerID() const { return m_dwServerID; }
	void SetServerID(uint32 val) { m_dwServerID = val; }

protected:
	virtual void handle_msgv(const void* pMsg);
	virtual void handle_msg(const tagMsgHead* pMsg) = 0;
	uint32 m_dwServerID = 0;
};