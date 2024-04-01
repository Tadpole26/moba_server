#include "CServerNetface.h"
#include "msg_module.pb.h"
#include "msg_module_servercommon.pb.h"
#include "log_mgr.h"
#include "parse_pb.h"
#include "CSessionMgr.h"
#include "CCrossLogic.h"
using namespace ProtoMsg;

CClientLogic::CClientLogic()
{
}

CClientLogic::~CClientLogic()
{
}

//msg from gameserver
void CClientLogic::handle_logic_msg(const tagNetMsg* pNetMsg)
{
	const tagHostHd& hd = pNetMsg->m_hd;
	tagMsgHead* pMsgHead = NET_HEAD_MSG(pNetMsg->m_body);

	switch (pMsgHead->usModuleId)
	{
	case MsgModule::ServerInner:
	{
		CSvrLogicFace::handle_logic_msg(pNetMsg);
	}
	break;
	case MsgModule::ServerCommon:
	{
		HandleServerCommon(pMsgHead, hd);
	}
	break;
	default:
		break;
	}
}

void CClientLogic::HandleServerCommon(const tagMsgHead* pMsgHead, const tagHostHd& hd)
{
	switch (pMsgHead->uiCmdId)
	{
	case MsgModule_ServerCommon::Msg_ServerCommon_RegisterReq:
	{
		OnServerRegister(pMsgHead, hd);
	}
	break;
	case MsgModule_ServerCommon::Msg_ServerCommon_BeatHartReq:
	{
		if (!has_session(hd)) return;

		Msg_ServerCommon_BeatHartReq oBeatReq;
		tagMsgHead* pNetMsgHead = MakeHeadMsg(&oBeatReq, ServerCommon
			, MsgModule_ServerCommon::Msg_ServerCommon_BeatHartReq, 0);
		if (pNetMsgHead != nullptr)
			my_send_conn_msg(m_pDispatcher, hd.m_threadOid, hd.m_connOid, pNetMsgHead);
	}
	break;
	default:
		Log_Error("error cmd:%u", pMsgHead->uiCmdId);
		break;
	}
}

void CClientLogic::OnServerRegister(const tagMsgHead* pMsgHead, const tagHostHd& hd)
{
	Msg_ServerCommon_RegisterReq oRegisterReq;
	PARSE_PTL_HEAD(oRegisterReq, pMsgHead);

	if (oRegisterReq.uiserverkind() == SERVER_KIND_GAME)
	{
		if (GAME_SESSIONS_INS->GetGameById(oRegisterReq.uiserverid()) != nullptr)
			return;

		CGameSession* pSession = GAME_SESSIONS_INS->AddGameSvr(oRegisterReq.uiserverid()
			, oRegisterReq.uigroupid());
		if (pSession == nullptr) return;

		pSession->m_uiPlatId = oRegisterReq.uiplatid();
		pSession->m_uiIndex = oRegisterReq.uiindex();
		pSession->m_strExIp = oRegisterReq.strexip();

		CROSS_LOGIC_INS->m_pClientLIF->add_session(pSession, hd, MAX_SIZE_512M);

		Msg_ServerCommon_RegisterRsp oRegisterRsp;
		oRegisterRsp.set_uiserverkind(SERVER_KIND_WORLD);
		oRegisterRsp.set_uiserverid(CROSS_LOGIC_INS->m_oConstCfg.m_uiGroupId);
		pSession->Send_Msg(&oRegisterRsp, MsgModule_ServerCommon::Msg_ServerCommon_RegisterRsp
			, MsgModule::ServerCommon);
		Log_Info("accept game server id:%u, kind:%d succeed!", oRegisterRsp.uiserverid()
			, oRegisterRsp.uiserverkind());
	}
	else
	{
		Log_Error("wrong server kind:%d!", oRegisterReq.uiserverkind());
	}
}