#include "CSysNetface.h"
#include "../protocol/msg_module_serverinner.pb.h"
#include "../protocol/msg_module_servercommon.pb.h"
#include "../protocol/msg_module_login.pb.h"
#include "../protocol/server_common.pb.h"
#include "util_time.h"
#include "CGateLogic.h"
#include "global_define.h"
#include "parse_pb.h"
#include "PlayerProxy.h"
#include "CUser.h"
#include "CServerNetface.h"
using namespace ProtoMsg;

CSysNetface::CSysNetface()
{
}

CSysNetface::~CSysNetface()
{
}

void CSysNetface::UpdateOnlinePlayerNum(uint32_t uiSvrId, uint32_t uiOnline,
	uint32_t uiWaitNum, std::string strIp, uint32 uiPort, uint32 uiAreaNo)
{
	Msg_ServerInner_GS_RecordOnline oRecord;
	oRecord.set_uiserverid(uiSvrId);
	oRecord.set_llcurtime(GetCurrTime());
	oRecord.set_uionlinenum(uiOnline);
	oRecord.set_uiwaitnum(uiWaitNum);
	oRecord.set_strip(strIp);
	oRecord.set_uiport(uiPort);
	oRecord.set_uiareano(uiAreaNo);
	oRecord.set_uimaxnum(gGateLogic->m_oConstConfig.m_uiLimit);
	oRecord.set_strversion(gGateLogic->GetResVersion());

	Send_Msg(&oRecord, MsgModule_ServerInner::Msg_ServerInner_GS_RecordOnline,
		MsgModule::ServerInner);
}

void CSysNetface::UpdateOnlinePlayerNum()
{
	if (!gGateLogic->m_oUserMgr.GetLoadFinish())
		return;

	UpdateOnlinePlayerNum(gGateLogic->GetIndex(), (uint32)gGateLogic->m_oUserMgr.GetInGamePlayerSize(),
		(uint32)gGateLogic->m_oUserMgr.GetWaitCheckPlayerSize(), gGateLogic->m_oConstConfig.m_strGateIp,
		gGateLogic->m_oConstConfig.m_uiGatePort, gGateLogic->m_oConstConfig.m_uiGroupId);
}

void CSysNetface::CleanOnlineNum(uint32_t uiSvrId, uint32_t uiAreaNo)
{
	Msg_ServerInner_GS_ClearOnline oRecord;
	oRecord.set_uiserverid(uiSvrId);
	oRecord.set_uiareano(uiAreaNo);
	oRecord.set_llcurtime(GetCurrTime());
	Send_Msg(&oRecord, MsgModule_ServerInner::Msg_ServerInner_GS_ClearOnline, MsgModule::ServerInner);
}

void CSysNetface::PlayerLogin(int64 llPlayerId, std::string strSession)
{
	Msg_ServerInner_GS_PlayerLoginReq oLoginReq;
	oLoginReq.set_llplayerid(llPlayerId);
	oLoginReq.set_strsessionid(strSession);
	oLoginReq.set_uiserverid(GetServerID());
	Send_Msg(&oLoginReq, MsgModule_ServerInner::Msg_ServerInner_GS_PlayerLoginReq, MsgModule::ServerInner);
}

void CSysNetface::NotifyPlayerOnline(uint32_t uiSvrId, int64 llPlayerId)
{
	Msg_ServerInner_GS_NotifyOnline oRecord;
	oRecord.set_uiserverid(uiSvrId);
	oRecord.set_llplayerid(llPlayerId);
	oRecord.set_llcurtime(GetCurrTime());
	Send_Msg(&oRecord, MsgModule_ServerInner::Msg_ServerInner_GS_NotifyOnline, MsgModule::ServerInner);
}

void CSysNetface::NotifyPlayerOffline(uint32_t uiSvrId, int64 llPlayerId)
{
	Msg_ServerInner_GS_NotifyLogout oRecord;
	oRecord.set_uiserverid(uiSvrId);
	oRecord.set_llplayerid(llPlayerId);
	oRecord.set_llcurtime(GetCurrTime());
	Send_Msg(&oRecord, MsgModule_ServerInner::Msg_ServerInner_GS_NotifyLogout, MsgModule::ServerInner);
}

void CSysNetface::trigger()
{
	static Msg_ServerCommon_BeatHart_Req oBeatHart;
	Send_Msg(&oBeatHart, MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Req, MsgModule::ServerCommon);
}

void CSysNetface::SendWChatMsg(const std::string& strClient,
	const std::string& strServer, const std::string& strMsg)
{
	Msg_ServerInner_GS_WMsg oProtol;
	oProtol.set_strmsg(strMsg);
	oProtol.set_strclient(strClient);
	oProtol.set_strserver(strServer);
	Send_Msg(&oProtol, MsgModule_ServerInner::Msg_ServerInner_GS_WMsg, MsgModule::ServerInner);
}

void CSysNetface::handle_msg(const tagMsgHead* pMsg)
{
	if (nullptr == pMsg) return;

	switch (pMsg->usModuleId)
	{
	case MsgModule::ServerCommon:
		HandleServerCommon(pMsg);
		break;
	case MsgModule::ServerInner:
		HandleSysLogModule(pMsg);
		break;
	default:
		Log_Error("unknown module:%u", pMsg->usModuleId);
		break;
	}
}

void CSysNetface::on_connect()
{
	Msg_ServerCommon_Register_Req oRegister;
	oRegister.set_uiserverid(GetServerID());
	oRegister.set_uiserverkind(SERVER_KIND::SERVER_KIND_GATE);
	oRegister.set_uiplatid(gGateLogic->m_oConstConfig.m_uiPlatId);
	oRegister.set_uiindex(gGateLogic->GetIndex());
	oRegister.set_uigroupid(gGateLogic->m_oConstConfig.m_uiGroupId);
	Send_Msg(&oRegister, MsgModule_ServerCommon::Msg_ServerCommon_Register_Req, MsgModule::ServerCommon);
	CTcpReconn::on_connect();
	//if (get_dicon_time() != 0)
	//	GATE_LOGIC_INS->SendWChatMsg("Gate", "Sys", "connect");
	//else
	//	CleanOnlineNum(GATE_LOGIC_INS->GetIndex(), GATE_LOGIC_INS->m_oConstConfig.m_uiGroupId);

	UpdateOnlinePlayerNum();
}

void CSysNetface::on_disconnect()
{
	gGateLogic->SendWChatMsg("Gate", "Sys", "disconnect");
	Log_Warning("sys server disconnect! ");
}


void CSysNetface::HandleServerCommon(const tagMsgHead* pMsgHead)
{
	switch (pMsgHead->uiCmdId)
	{
	case MsgModule_ServerCommon::Msg_ServerCommon_Register_Rsp:
	{
		Msg_ServerCommon_Register_Rsp oRegRsp;
		PARSE_PTL_HEAD(oRegRsp, pMsgHead);
		Log_Info("connect sys server id:%u kind:%u succeed!", 
			oRegRsp.uiserverid(), oRegRsp.uiserverkind());
	}
	break;
	case MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Rsp:
	{
	}
	break;
	case MsgModule_ServerCommon::Msg_ServerCommon_Transmit:
	{
		CPlayerProxy::TransimitToClient(pMsgHead);
	}
	break;
	default:
	{
		Log_Error("unknown assistant proto id:%u", pMsgHead->uiCmdId);
	}
	break;
	}
}
void CSysNetface::HandleSysLogModule(const tagMsgHead* pMsgHead)
{
	switch (pMsgHead->uiCmdId)
	{
	case MsgModule_ServerInner::Msg_ServerInner_GS_PlayerLoginRsp:
	{
		OnPlayerLoginRet(pMsgHead);
	}
	break;
	case MsgModule_ServerInner::Msg_ServerInner_GS_KickPlayerNotify:
	{
		OnKickPlayer(pMsgHead);
	}
	break;
	default:
		Log_Error("Unknown assistant proto id:%u", pMsgHead->uiCmdId);
		break;
	}
}

void CSysNetface::OnPlayerLoginRet(const tagMsgHead* pMsgHead)
{
	Msg_ServerInner_GS_PlayerLoginRsp oLoginRsp;
	PARSE_PTL_HEAD(oLoginRsp, pMsgHead);

	int64 llUserId = oLoginRsp.llplayerid();
	ResultCode eRet = oLoginRsp.eresult();
	uint32 uiGameId = oLoginRsp.uigameid();

	CUser* pUser = gGateLogic->m_oUserMgr.GetCheckingPwdPlayer(llUserId);
	if (!pUser)
	{
		Log_Error("user not exist! user:%lld", llUserId);
		return;
	}

	if (pUser->m_strSessionId != oLoginRsp.strsessionid())
	{
		Log_Custom("enter", "check pwd error player id:%lld, game id:%u, ms:%lld",
			llUserId, uiGameId, pUser->m_oBeginTime.Ms());
		return;
	}

	if (ResultCode::Code_Common_Success != eRet)
	{
		Log_Custom("enter", "check error player id:%lld, result:%u, game id:%u, ms:%lld",
			llUserId, eRet, uiGameId, pUser->m_oBeginTime.Ms());
	}
	else
	{
		Log_Custom("enter", "check success user id:%lld, result:%u, game id:%u, ms:%lld, area:%u, province:%s, citycode:%s",
			llUserId, eRet, uiGameId, pUser->m_oBeginTime.Ms(), oLoginRsp.uiproarea(),
			oLoginRsp.strprovincename().c_str(), oLoginRsp.strcitycode().c_str());
	}

	pUser->m_oBeginTime.ResetEnd();

	if (ResultCode::Code_Common_Success != eRet)
	{
		Msg_Login_VerifyAccount_Rsp oRsp;
		oRsp.set_lluserid(llUserId);
		oRsp.set_strusername(pUser->m_strAccName);
		oRsp.set_iflag((ResultCode)eRet);
		oRsp.set_uigameid(uiGameId);
		oRsp.set_llservertime(GetCurrTime());
		oRsp.set_itimezone(GetTimeZone());
		oRsp.set_llopendate(0);
		oRsp.set_estatus(EPlayerStatus::ePlayer_Offline);
		oRsp.set_strmsgkey(pUser->m_strMsgKey);
		oRsp.set_uimsgrand(pUser->m_uiMsgRand);
		pUser->SendToClient(&oRsp, MsgModule::Login,
			MsgModule_Login::Msg_Login_VerifyAccount_Rsp, pUser->m_uiSequence,
			(ResultCode)oRsp.iflag());

		Log_Info("kick player:%lld, ret code:%u!", llUserId, eRet);
		gGateLogic->m_pClientLIF->OnKickConnect(pUser, false);
	}
	else
	{
		pUser->m_strDevId = oLoginRsp.strdevid();
		pUser->m_uiProvince = oLoginRsp.uiprovince();
		pUser->m_strProvince = oLoginRsp.strprovincename();
		pUser->m_uiProArea = oLoginRsp.uiproarea();
		pUser->m_strChannel = oLoginRsp.strchannel();
		pUser->m_strCityCode = oLoginRsp.strcitycode();
		if (!oLoginRsp.strplayername().empty())
			pUser->m_strAccName = oLoginRsp.strplayername();

		CPlayerProxy::LogicToGameServer(pUser, uiGameId);
	}
}

void CSysNetface::OnKickPlayer(const tagMsgHead* pMsgHead)
{
	Msg_ServerInner_GS_KickPlayerNotify oKickNotify;
	PARSE_PTL_HEAD(oKickNotify, pMsgHead);

	int64 llUid = oKickNotify.llplayerid();
	CUser* pUser = gGateLogic->m_oUserMgr.GetPlayer(llUid);
	if (pUser == nullptr)
	{
		Log_Error("player not exist! player:%lld", llUid);
		return;
	}
	Log_Info("kick player:%lld!", llUid);
	Log_Warning("Msg_Kick_Notify Disconnect last connection! player name:%s, player id:%lld",
		pUser->m_strAccName.c_str(), llUid);
	gGateLogic->m_pClientLIF->OnKickConnect(pUser, false);
}