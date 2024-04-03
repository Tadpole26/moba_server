#include "CGameNetface.h"
#include "msg_module_serverinner.pb.h"
#include "msg_module_servercommon.pb.h"
#include "msg_module_login.pb.h"
#include "server_common.pb.h"
#include "util_time.h"
#include "CGateLogic.h"
#include "global_define.h"
#include "parse_pb.h"
#include "PlayerProxy.h"
#include "CUser.h"
#include "CServerNetface.h"
#include "CSysNetface.h"

CGameNetface::CGameNetface()
{
}

CGameNetface::~CGameNetface() 
{ 
}

void CGameNetface::handle_msg(const tagMsgHead* pMsg)
{
	if (nullptr == pMsg) return;

	switch (pMsg->usModuleId)
	{
		//GameServer过来的通用消息(game->gate->client)
	case ProtoMsg::MsgModule::ServerCommon:
		HandleServerCommon(pMsg);
		break;
		//GameServer过来的内部消息(game->gate)
	case ProtoMsg::MsgModule::ServerInner:
		HandleGameModule(pMsg);
		break;
	default:
		Log_Error("undefined module %u!", pMsg->usModuleId);
		break;
	}
}

void CGameNetface::on_connect()
{
	if (get_dicon_time() != 0) gGateLogic->SendWChatMsg("Gate", "Game", "connect");

	Msg_ServerCommon_Register_Req oRegisterReq;
	oRegisterReq.set_uiserverid(GetServerID());
	oRegisterReq.set_uiserverkind(SERVER_KIND_GATE);
	oRegisterReq.set_uiplatid(gGateLogic->m_oConstConfig.m_uiPlatId);
	oRegisterReq.set_uiindex(gGateLogic->GetIndex());
	oRegisterReq.set_uigroupid(gGateLogic->m_oConstConfig.m_uiGroupId);
	Send_Msg(&oRegisterReq, MsgModule_ServerCommon::Msg_ServerCommon_Register_Req, MsgModule::ServerCommon);
	CTcpReconn::on_connect();
}

void CGameNetface::on_disconnect()
{
	gGateLogic->SendWChatMsg("Gate", "Game", "disconnect");
	Log_Warning("game server disconnect!");
}

void CGameNetface::trigger()
{
	static ProtoMsg::Msg_ServerCommon_BeatHart_Req oBeatHartReq;
	Send_Msg(&oBeatHartReq, MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Req, MsgModule::ServerCommon);
}

void CGameNetface::HandleServerCommon(const tagMsgHead* pMsgHead)
{
	switch (pMsgHead->uiCmdId)
	{
	case ProtoMsg::MsgModule_ServerCommon::Msg_ServerCommon_Register_Rsp:
	{
		ProtoMsg::Msg_ServerCommon_Register_Rsp oRegisterRsp;
		PARSE_PTL_HEAD(oRegisterRsp, pMsgHead);

		gGateLogic->m_oUserMgr.SetLoadFinish(oRegisterRsp.bfirstcon());
		gGateLogic->SetResVersion(oRegisterRsp.strcomment());
		gGateLogic->m_pSysNetface->UpdateOnlinePlayerNum();
		Log_Info("connect game server id:%u, kind:%d succeed!", 
			oRegisterRsp.uiserverid(), oRegisterRsp.uiserverkind());
	}
	break;
	case ProtoMsg::MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Rsp:
		break;
	case ProtoMsg::MsgModule_ServerCommon::Msg_ServerCommon_Transmit:
		CPlayerProxy::TransimitToClient(pMsgHead);
		break;
	default:
		Log_Error("undefined cmd %u!", pMsgHead->uiCmdId);
		break;
	}
}

void CGameNetface::HandleGameModule(const tagMsgHead* pNetMsgHead)
{
	switch (pNetMsgHead->uiCmdId)
	{
	case ProtoMsg::MsgModule_ServerInner::Msg_ServerInner_GG_Login_Rsp:
		OnAccountLoginRet(pNetMsgHead);
		break;
	case ProtoMsg::MsgModule_ServerInner::Msg_ServerInner_GG_Create_Rsp:
		OnCreateRoleRet(pNetMsgHead);
		break;
	case ProtoMsg::MsgModule_ServerInner::Msg_ServerInner_GG_Kick_Notify:
		OnKickPlayer(pNetMsgHead);
		break;
	case ProtoMsg::MsgModule_ServerInner::Msg_ServerInner_GG_Msg_Notify:
		OnNoticePush(pNetMsgHead);
		break;
	default:
		Log_Error("undefined cmd %u!", pNetMsgHead->uiCmdId);
		break;
	}
}

void CGameNetface::OnAccountLoginRet(const tagMsgHead* pNetMsgHead)
{
	Msg_ServerInner_GG_Login_Rsp oLoginRsp;
	PARSE_PTL_HEAD(oLoginRsp, pNetMsgHead);

	CUser* pUser = gGateLogic->m_oUserMgr.GetInGamePlayer(oLoginRsp.lluserid());
	if (pUser == nullptr)
		pUser = gGateLogic->m_oUserMgr.AddInGamePlayer(oLoginRsp.lluserid());

	if (pUser == nullptr)
	{
		Log_Error("Add in game player failed! account id:%lld", oLoginRsp.lluserid());
		return;
	}

	ProtoMsg::Msg_Login_VerifyAccount_Rsp oVerifyRsp;
	oVerifyRsp.set_lluserid(oLoginRsp.lluserid());
	oVerifyRsp.set_strusername(pUser->m_strAccName);
	oVerifyRsp.set_iflag(oLoginRsp.eflag());
	oVerifyRsp.set_uigameid(pUser->m_uiGameId);
	oVerifyRsp.set_llservertime(GetCurrTime());
	oVerifyRsp.set_itimezone(GetTimeZone());
	oVerifyRsp.set_llopendate(oLoginRsp.llopendate());
	oVerifyRsp.set_estatus(oLoginRsp.estate());
	oVerifyRsp.set_strmsgkey(pUser->m_strMsgKey);
	oVerifyRsp.set_uimsgrand(pUser->m_uiMsgRand);

	pUser->SendToClient(&oVerifyRsp, MsgModule::Login,
		ProtoMsg::MsgModule_Login::Msg_Login_VerifyAccount_Rsp, oLoginRsp.iseqid(),
		(ProtoMsg::ResultCode)oLoginRsp.eflag());

	if (ResultCode::Code_Common_Success == oLoginRsp.eflag())
	{
		CPlayerProxy::NotifyPlayerOnline(pUser);
		gGateLogic->m_oUserMgr.PushThreadConn(pUser);
		pUser->m_llUid = oLoginRsp.lluserid();
		Log_Custom("enter", "user success name=%s, account id=%lld, ms=%lld",
			pUser->m_strAccName.c_str(), oLoginRsp.lluserid(),
			pUser->m_oBeginTime.Ms());
	}
	else
	{
		Log_Custom("enter", "user error name=%s, id=%lld, ms=%lld, reuslt=%d", 
			pUser->m_strAccName.c_str(), oLoginRsp.lluserid(),
			pUser->m_oBeginTime.Ms(), oLoginRsp.eflag());
	}
	pUser->m_oBeginTime.ResetEnd();
}

void CGameNetface::OnCreateRoleRet(const tagMsgHead* pNetMsgHead)
{
	Msg_ServerInner_GG_Create_Rsp oSysCreateRsp;
	PARSE_PTL_HEAD(oSysCreateRsp, pNetMsgHead);

	CUser* pUser = gGateLogic->m_oUserMgr.GetInGamePlayer(oSysCreateRsp.lluserid());
	if (pUser == nullptr)
		pUser = gGateLogic->m_oUserMgr.AddInGamePlayer(oSysCreateRsp.lluserid());

	if (!pUser)
	{
		Log_Error("Add in game player failed! account id:lld", oSysCreateRsp.lluserid());
		return;
	}

	Msg_Login_CreateUser_Rsp oCreateRsp;
	oCreateRsp.set_lluserid(oSysCreateRsp.lluserid());
	oCreateRsp.set_strusername(oSysCreateRsp.strusername());
	oCreateRsp.set_uilevel(oSysCreateRsp.ilevel());
	oCreateRsp.set_llcreatetime(oSysCreateRsp.llcreatetime());
	pUser->SendToClient(&oCreateRsp, MsgModule::Login, ProtoMsg::MsgModule_Login::Msg_Login_CreateUser_Rsp,
		oSysCreateRsp.iseqid(), (ProtoMsg::ResultCode)oSysCreateRsp.ecode());

	if (ResultCode::Code_Common_Success == oSysCreateRsp.ecode())
	{
		gGateLogic->m_oUserMgr.PushThreadConn(pUser);

		CPlayerProxy::NotifyPlayerOnline(pUser);
		pUser->m_llUid = oSysCreateRsp.lluserid();
	}
	else
	{
		if (oSysCreateRsp.ecode() == Code_Login_RoleNameCollision)
		{
			Log_Warning("accid:%lld, rolename:%s, error:%d",
				pUser->m_llUid,
				pUser->m_strCreateName.c_str(),
				oSysCreateRsp.ecode());
		}
		else
		{
			Log_Error("addid:%lld, error:%d", pUser->m_llUid, oSysCreateRsp.ecode());
		}
	}
}

void CGameNetface::OnKickPlayer(const tagMsgHead* pNetMsgHead)
{
	Msg_ServerInner_GG_Kick_Notify oKickNotify;
	PARSE_PTL_HEAD(oKickNotify, pNetMsgHead);

	CUser* pUser = gGateLogic->m_oUserMgr.GetInGamePlayer(oKickNotify.lluserid());
	if (pUser == nullptr) return;
	Log_Info("kick user id %lld!", pUser->m_llUid);

	//Msg_Login_KickPlayer_Notify stKickRsp;
	//stKickRsp.set_llaccid(pGamePlayer->m_llPlayerId);
	//stKickRsp.set_straccname(pGamePlayer->m_strAccName);
	//stKickRsp.set_iflag(oKickNotify.ecode());
	//stKickRsp.set_uigameid(pGamePlayer->m_uiGameId);
	//pGamePlayer->SendToClient(&stKickRsp, MsgModule::Login,
	//	ProtoMsg::MsgModule_Login::Msg_Login_KickRole_Notify, 0, oKickNotify.ecode());
	Log_Warning("disconnect last connection! account name:%s, user id :%lld",
		pUser->m_strAccName.c_str(), pUser->m_llUid);
	gGateLogic->m_pClientLIF->OnKickConnect(pUser, true);
}

void CGameNetface::OnNoticePush(const tagMsgHead* pNetMsgHead)
{
	Msg_ServerInner_GG_Msg_Notify oMsgNotify;
	PARSE_PTL_HEAD(oMsgNotify, pNetMsgHead);

	static CMsgMake oMakeMsg;
	tagMsgHead* pMakeMsgHead = oMakeMsg.MakeHeadMsg(oMsgNotify.strcmdmsg(),
		oMsgNotify.uimoduleid(), oMsgNotify.uicmdid(), 0);
	if (pMakeMsgHead == nullptr)
	{
		Log_Error("module:%u, cmd:%u", oMsgNotify.uimoduleid(), oMsgNotify.uicmdid());
		return;
	}

	if (oMsgNotify.vecplayerid_size() > 0)
	{
		for (const auto& value : oMsgNotify.vecplayerid())
		{
			gGateLogic->m_oUserMgr.SendToPlayer(value, pMakeMsgHead);
		}
	}
	else
	{
		//未设置发送间隔
		if (oMsgNotify.uiinterval() == 0)
			gGateLogic->m_oUserMgr.SendToAllPlayer(pMakeMsgHead);
		else
			gGateLogic->m_oUserMgr.SendNoticeAllPlayer(oMsgNotify.strcmdmsg(),
				oMsgNotify.uimoduleid(), oMsgNotify.uicmdid(), oMsgNotify.uiinterval());
	}
}
