#include "CServerNetface.h"
#include "CBattleSession.h"
#include "log_mgr.h"
#include "msg_module_serverbattle.pb.h"
#include "msg_module_servercommon.pb.h"
#include "parse_pb.h"
#include "CBattleManager.h"
#include "CCrossSession.h"
#include "CBCenterLogic.h"
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
	unsigned char* pBuf = NET_DATA_BUF(pNetMsg);
	size_t uiLen = NET_DATA_SIZE(pMsgHead);

	switch (pMsgHead->usModuleId)
	{
	case ProtoMsg::ServerInner:
	{
		CSvrLogicFace::handle_logic_msg(pNetMsg);
	}
	break;
	case ProtoMsg::ServerBattle:
	{
		HandleBattleServerMsg(pBuf, uiLen, pMsgHead->uiCmdId, stHead);
	}
	break;
	case ProtoMsg::ServerCommon:
	{
		HandleServerCommonMsg(pMsgHead, stHead);
	}
	break;
	default:
		Log_Error("undefined module:%d, cmd:%u", pMsgHead->usModuleId, pMsgHead->uiCmdId);
		break;
	}
}

void CClientLogic::HandleBattleServerMsg(unsigned char* pMsg, size_t uiSize, uint32_t uiCmd, const tagHostHd& stHead)
{
	switch (uiCmd)
	{
	case MsgModule_ServerBattle::Msg_ServerBattle_BC_Register_Req:
	{
		OnBattleServerRegister(pMsg, uiSize, stHead);
	}
	break;
	default:
	{
		CBattleSession* pSession = dynamic_cast<CBattleSession*>(get_session(stHead));
		if (!pSession)
		{
			Log_Error("battle session is nullptr, cmd:%u", uiCmd);
			return;
		}
		pSession->handle_msg(uiCmd, pMsg, uiSize);
	}
	break;
	}
}

void CClientLogic::HandleServerCommonMsg(tagMsgHead* pMsgHead, const tagHostHd& stHead)
{
	switch (pMsgHead->uiCmdId)
	{
	case MsgModule_ServerCommon::Msg_ServerCommon_Register_Req:
	{
		OnCrossServerRegister(pMsgHead, stHead);
	}
	break;
	case MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Req:
	{
		if (!has_session(stHead))
			return;

		Msg_ServerCommon_BeatHart_Rsp oRsp;
		tagMsgHead* pNetMsgHead = MakeHeadMsg(&oRsp, ServerCommon, MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Rsp, 0);
		if (pNetMsgHead != nullptr)
			my_send_conn_msg(m_pDispatcher, stHead.m_threadOid, stHead.m_connOid, pNetMsgHead);
	}
	break;
	default:
	{
		Log_Warning("error cmd:%u", pMsgHead->uiCmdId);
	}
	break;
	}
}

void CClientLogic::OnBattleServerRegister(unsigned char* pMsg, size_t uiSize, const tagHostHd& stHead)
{
	Msg_ServerBattle_BC_Register_Req oRegisterReq;
	PARSE_PTL(oRegisterReq, pMsg, uiSize);

	CBattleSession* pSession = nullptr;
	pSession = gBattleManager->GetBattleServerById(oRegisterReq.iid());
	if (pSession != nullptr)
		return;

	pSession = gBattleManager->AddBattleSvr(oRegisterReq.iid(), oRegisterReq.imaxroomnum());
	if (pSession == nullptr)
		return;

	pSession->Init(oRegisterReq);
	gBCenterLogic->m_pServerLIF->add_session(pSession, stHead, MAX_SIZE_512M);
	gBattleManager->SetBattleGroupIp(oRegisterReq.iserverarea(), oRegisterReq.strip());

	Msg_ServerBattle_BC_Register_Rsp oRegisterRsp;
	oRegisterRsp.set_iserverid(gBCenterLogic->GetIndex());
	pSession->Send_Msg(&oRegisterRsp, MsgModule_ServerBattle::Msg_ServerBattle_BC_Register_Rsp, ProtoMsg::ServerBattle);
	//弹出等待队列
	pSession->PushWaitLst();

	Log_Custom("battlesvr", "id: %u, ip : %s, port : %d succeed!",
		pSession->GetServerId(), oRegisterReq.strip().c_str(), oRegisterReq.iport());
}

void CClientLogic::OnCrossServerRegister(tagMsgHead* pMsgHead, const tagHostHd& stHead)
{
	Msg_ServerCommon_Register_Req oRegisterReq;
	PARSE_PTL_HEAD(oRegisterReq, pMsgHead);

	if (gBCenterLogic->m_pCrossSession != nullptr && 
		gBCenterLogic->m_pCrossSession->GetServerId() == oRegisterReq.uiserverid())
	{
		Log_Warning("cross server id:%u already connected!", oRegisterReq.uiserverid());
		return;
	}

	gBCenterLogic->m_pCrossSession = new CCrossSession();
	if (!gBCenterLogic->m_pCrossSession)
	{
		Log_Warning("cross server id:%u new error!", oRegisterReq.uiserverid());
		return;
	}
	gBCenterLogic->m_pCrossSession->SetServerKind((SERVER_KIND)oRegisterReq.uiserverkind());
	gBCenterLogic->m_pCrossSession->SetServerId(oRegisterReq.uiserverid());

	gBCenterLogic->m_pServerLIF->add_session(gBCenterLogic->m_pCrossSession, stHead, MAX_SIZE_512M);
	Log_Info("accept cross server id:%u, kind:%d succeed!", oRegisterReq.uiserverid(),
		oRegisterReq.uiserverkind());

	Msg_ServerCommon_Register_Rsp oRegisterRsp;
	oRegisterRsp.set_uiserverid(gBCenterLogic->GetIndex());
	oRegisterRsp.set_uiserverkind(SERVER_KIND_FBATTLE);
	gBCenterLogic->m_pCrossSession->Send_Msg(&oRegisterRsp, MsgModule_ServerCommon::Msg_ServerCommon_Register_Rsp);
}