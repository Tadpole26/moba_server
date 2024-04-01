#include "CServerNetface.h"
#include "../Libs/protocol/msg_module.pb.h"
#include "../Libs/protocol/msg_module_servercommon.pb.h"
#include "log_mgr.h"
#include "parse_pb.h"
#include "CSession.h"
#include "CSysLogic.h"

using namespace ProtoMsg;

CClientLogic::CClientLogic()
{
}

CClientLogic::~CClientLogic()
{
}

void CClientLogic::handle_logic_msg(const tagNetMsg* pNetMsg)
{
	const tagHostHd& hd = pNetMsg->m_hd;
	tagMsgHead* pNetMsgHead = NET_HEAD_MSG(pNetMsg->m_body);
	if (nullptr == pNetMsgHead) return;

	switch (pNetMsgHead->usModuleId)
	{
		case ProtoMsg::MsgModule::ServerCommon:
		{
			HandleServerCommon(pNetMsgHead, hd);
		}
		break;
		default:
		{
			CSvrLogicFace::handle_logic_msg(pNetMsg);
		}
		break;
	}
}

void CClientLogic::HandleServerCommon(tagMsgHead* pMsgHead, const tagHostHd& hd)
{
	switch(pMsgHead->uiCmdId)
	{
	case MsgModule_ServerCommon::Msg_ServerCommon_Register_Req:
	{
		OnRegisterServer(pMsgHead, hd);
	}
	break;
	case MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Req:
	{
		if (!has_session(hd)) return;

		Msg_ServerCommon_BeatHart_Rsp oBeatRsp;
		tagMsgHead* pNetMsgHead = MakeHeadMsg(&oBeatRsp, ServerCommon,
			MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Rsp, 0);
		if (pNetMsgHead != nullptr)
			my_send_conn_msg(m_pDispatcher, hd.m_threadOid, hd.m_connOid, pNetMsgHead);
	}
	break;
	default:
	{
		Log_Error("undefined protocol %u!", pMsgHead->uiCmdId);
	}
	break;
	}
}

void CClientLogic::OnRegisterServer(tagMsgHead* pMsgHead, const tagHostHd& hd)
{
	Msg_ServerCommon_Register_Req oRegisterReq;
	PARSE_PTL_HEAD(oRegisterReq, pMsgHead);

	CSession* pServer = g_SvrMgr.GetServer(oRegisterReq.uiserverkind(), oRegisterReq.uiserverid());

	if (pServer)
	{
		Log_Warning("server kind:%u id:%u already connected!",
			oRegisterReq.uiserverkind(), oRegisterReq.uiserverid());
		return;
	}

	pServer = g_SvrMgr.AddServer(oRegisterReq.uiserverkind(), oRegisterReq.uiserverid());
	if (pServer)
	{
		add_session(pServer, hd, MAX_SIZE_512M);
		Log_Info("accept server id:%u, kind:%d succeed!",
			oRegisterReq.uiserverid(), oRegisterReq.uiserverkind());

		Msg_ServerCommon_Register_Rsp oRegisterRsp;
		oRegisterRsp.set_uiserverid(SYS_LOGIC_INS->m_oConstCfg.m_uiGroupId);
		oRegisterRsp.set_uiserverkind(SERVER_KIND_SYS);
		pServer->Send_Msg(&oRegisterRsp, MsgModule_ServerCommon::Msg_ServerCommon_Register_Rsp, MsgModule::ServerCommon);
	}
}