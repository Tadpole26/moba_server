#include "CServerNetface.h"
#include "msg_module.pb.h"
#include "msg_module_servercommon.pb.h"
#include "log_mgr.h"
#include "parse_pb.h"
#include "CGameSession.h"
#include "CDBLogic.h"
using namespace ProtoMsg;

CClientLogic::CClientLogic()
{
}

CClientLogic::~CClientLogic()
{
}

void CClientLogic::handle_logic_msg(const tagNetMsg* pNetMsg)
{
	const tagHostHd& stHead = pNetMsg->m_hd;
	tagMsgHead* pMsgHead = NET_HEAD_MSG(pNetMsg->m_body);

	switch (pMsgHead->usModuleId)
	{
	case MsgModule::ServerCommon:
	{
		HandleServerCommon(pMsgHead, stHead);
	}
	break;
	default:
	{
		CSvrLogicFace::handle_logic_msg(pNetMsg);
	}
	break;
	}
}

void CClientLogic::HandleServerCommon(tagMsgHead* pMsgHead, const tagHostHd& stHead)
{
	switch (pMsgHead->uiCmdId)
	{
	case MsgModule_ServerCommon::Msg_ServerCommon_Register_Req:
	{
		OnRegisterServer(pMsgHead, stHead);
	}
	break;
	case MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Req:
	{
		if (!has_session(stHead)) return;

		Msg_ServerCommon_BeatHart_Rsp oBeatRsp;
		tagMsgHead* pNetMsgHead = MakeHeadMsg(&oBeatRsp, MsgModule::ServerCommon, 
			MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Rsp, 0);
		if (pNetMsgHead != nullptr)
			my_send_conn_msg(m_pDispatcher, stHead.m_threadOid, stHead.m_connOid, pNetMsgHead);
	}
	break;
	default:
		Log_Error("can not defined cmd:%u", pMsgHead->uiCmdId);
		break;
	}
}

void CClientLogic::OnRegisterServer(tagMsgHead* pMsgHead, const tagHostHd& stHead)
{
	Msg_ServerCommon_Register_Req oRegisterReq;
	PARSE_PTL_HEAD(oRegisterReq, pMsgHead);

	CGameSession* pServer = gSvrManager.GetServer(oRegisterReq.uiserverkind(), oRegisterReq.uiserverid());
	if (pServer)
	{
		Log_Warning("game server id:%u already connected!", oRegisterReq.uiserverid());
		return;
	}
	pServer = gSvrManager.AddServer(oRegisterReq.uiserverkind(), oRegisterReq.uiserverid());
	if (pServer)
	{
		add_session(pServer, stHead, MAX_SIZE_512M);
		Log_Info("accept game server id:%u,kind:%u succeed!", oRegisterReq.uiserverid(), oRegisterReq.uiserverkind());

		Msg_ServerCommon_Register_Rsp oRegisterRsp;
		oRegisterRsp.set_uiserverid(gDBLogic->GetGroupId());
		oRegisterRsp.set_uiserverkind(SERVER_KIND_DB);
		pServer->Send_Msg(&oRegisterRsp, MsgModule_ServerCommon::Msg_ServerCommon_Register_Rsp);
	}
}