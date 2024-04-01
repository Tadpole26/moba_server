#include "pch.h"
#include "svr_client.h"
#include "log_mgr.h"
#include "global_define.h"

svr_session::svr_session() {}
svr_session::~svr_session() {}

bool svr_session::Send_Msg(google::protobuf::Message* pMsg, msg_id_t usProtocol
	, msg_id_t usModule, ProtoMsg::ResultCode code
	, uint32 seqid)
{
	if (pMsg == nullptr)
	{
		Log_Error("module:%u, cmd:%u, class:%s", usModule, usProtocol, typeid(*this).name());
		return false;
	}

	tagMsgHead* pNetMsgHead = MakeHeadMsg(pMsg, usModule, usProtocol, seqid, code);
	if (!pNetMsgHead)
	{
		Log_Error("pNetMsgHead is null!, module:%u, cmd:%u, class:%s"
			, usModule, usProtocol, typeid(*this).name());
		return false;
	}
	bool res = Send(pNetMsgHead);
	CMsgMake::ClearMakeCache();
	return res;
}

bool svr_session::Send_Msg(const std::string& strMsg, msg_id_t usProtocol
	, msg_id_t usModule, ProtoMsg::ResultCode code
	, uint32 seqid)
{
	return Send_Msg((const uchar*)strMsg.c_str(), strMsg.size(), usProtocol, usModule, code, seqid);
}

bool svr_session::Send_Msg(const uchar* buf, size_t size, msg_id_t usProtocol
	, msg_id_t usModule, ProtoMsg::ResultCode code
	, uint32 seqid)
{
	tagMsgHead* pNetMsgHead = MakeHeadMsg((const char*)buf, size, usModule, usProtocol, seqid, code);
	if (!pNetMsgHead)
	{
		Log_Error("pNetMsgHead is null!, module:%u, cmd:%u, class:%s"
			, usModule, usProtocol, typeid(*this).name());
		return false;
	}
	bool res = Send(pNetMsgHead);
	CMsgMake::ClearMakeCache();
	return res;
}

void svr_session::Init()
{
	m_hd.m_threadOid = invalid_thread_oid;
	m_hd.m_connOid = invalid_conn_oid;

	SetServerId(0);
	SetServerKind(SERVER_KIND_NONE);
}

void svr_session::Release()
{
	m_hd.m_threadOid = invalid_thread_oid;
	m_hd.m_connOid = invalid_conn_oid;

	SetServerId(0);
	SetServerKind(SERVER_KIND_NONE);
}

bool svr_session::Send(const tagMsgHead* pMsg)
{
	if (pMsg == nullptr) return false;

	bool bRes = CTcpSession::_Send(pMsg);
	if (!bRes)
	{
		Log_Error("pNetMsgHead is null!, module:%u, cmd:%u", pMsg->usModuleId, pMsg->uiCmdId);
		return false;
	}
	return bRes;
}

void svr_session::handle_msgv(const void* pMsg)
{
	handle_msg(NET_HEAD_MSG(pMsg));
}

////////////////////////////////////////////////////////////////////////////
svr_reconn::svr_reconn(bool bCache, uint32 CacheSec)
	: CTcpReconn(bCache, CacheSec)
{
}

svr_reconn::~svr_reconn()
{
}

bool svr_reconn::Send_Msg(google::protobuf::Message* pMsg, msg_id_t usProtocol
	, msg_id_t usModule, ProtoMsg::ResultCode code
	, uint32 seqid)
{
	if (pMsg == nullptr)
	{
		Log_Error("Moduleid:%u, cmd:%u, class:%s", usModule, usProtocol, typeid(*this).name());
		return false;
	}
	tagMsgHead* pNetMsgHead = MakeHeadMsg(pMsg, usModule, usProtocol, seqid, code);
	if (!pNetMsgHead)
	{
		Log_Error("pNetMsgHead is null!, module:%u, cmd:%u, class:%s"
			, usModule, usProtocol, typeid(*this).name());
		return false;
	}
	bool res = Send(pNetMsgHead);
	CMsgMake::ClearMakeCache();
	return res;
}

bool svr_reconn::Send_Msg(const std::string& strMsg, msg_id_t usProtocol
	, msg_id_t usModule, ProtoMsg::ResultCode code
	, uint32 seqid)
{
	return Send_Msg((const uchar*)strMsg.c_str()
		, strMsg.size(), usProtocol, usModule, code, seqid);
}

bool svr_reconn::Send_Msg(const uchar* buf, size_t size, msg_id_t usProtocol
	, msg_id_t usModule, ProtoMsg::ResultCode code
	, uint32 seqid)
{
	tagMsgHead* pNetMsgHead = MakeHeadMsg((const char*)buf, size, usModule, usProtocol, seqid, code);
	if (!pNetMsgHead)
	{
		Log_Error("pNetMsgHead is null!, module:%u, cmd:%u, class:%s"
			, usModule, usProtocol, typeid(*this).name());
		return false;
	}
	bool res = Send(pNetMsgHead);
	CMsgMake::ClearMakeCache();
	return res;
}

bool svr_reconn::Send(const tagMsgHead* pMsg)
{
	if (pMsg == nullptr) return false;
	bool bRes = CTcpReconn::_Send(pMsg);
	if (!bRes && is_connected())
	{
		Log_Error("pNetMsgHead is null!, module:%u, cmd:%u", pMsg->usModuleId, pMsg->uiCmdId);
		return false;
	}
	return bRes;
}

void svr_reconn::handle_msgv(const void* pMsg)
{
	handle_msg(NET_HEAD_MSG(pMsg));
}