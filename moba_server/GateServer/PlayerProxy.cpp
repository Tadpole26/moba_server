#include "PlayerProxy.h"
#include "CUser.h"
#include "CGateLogic.h"
#include "CSysNetface.h"
#include "CGameNetface.h"
#include "../protocol/msg_module_serverinner.pb.h"
#include "../protocol/msg_module_servercommon.pb.h"
#include "../protocol/msg_module_login.pb.h"
#include "../protocol/msg_common.pb.h"
#include "../serialize/parse_pb.h"
#include "../serialize/msg_make.h"
#include "util_string.h"
using namespace ProtoMsg;

void CPlayerProxy::CheckingSessionFromLoginSvr(CUser* pUser)
{
	if (pUser == nullptr) return;

	gGateLogic->m_pSysNetface->PlayerLogin(pUser->m_llUid, pUser->m_strSessionId);
}

void CPlayerProxy::LogicToGameServer(CUser* pUser, uint32_t uiGameId)
{
	if (pUser == nullptr) return;
	pUser->m_uiGameId = uiGameId;

	Msg_ServerInner_GG_Login_Req oLoginReq;
	oLoginReq.set_lluserid(pUser->m_llUid);
	oLoginReq.set_straccname(pUser->m_strAccName);
	oLoginReq.set_strsession(pUser->m_strSessionId);
	oLoginReq.set_uisdk(pUser->m_uiSdk);
	oLoginReq.set_strclientip(pUser->get_ip_str());
	oLoginReq.set_uirelogin(pUser->m_uiReLogin);
	oLoginReq.set_uiprovince(pUser->m_uiProvince);
	oLoginReq.set_strprovincename(pUser->m_strProvince);
	oLoginReq.set_iseqid(pUser->m_uiSequence);
	oLoginReq.set_strdeviceid(pUser->m_strDevId);
	oLoginReq.set_uiproarea(pUser->m_uiProArea);
	oLoginReq.set_strchannel(pUser->m_strChannel);
	oLoginReq.set_strcitycode(pUser->m_strCityCode);
	gGateLogic->m_pGameNetface->Send_Msg(&oLoginReq,
		MsgModule_ServerInner::Msg_ServerInner_GG_Login_Req, MsgModule::ServerInner);

	pUser->UpdateStatus(CUser::eUserStatus::STATUS_Checking);
	gGateLogic->m_oUserMgr.AddCheckingUser(pUser->m_llUid);
}

void CPlayerProxy::NotifyPlayerOnline(CUser* pUser)
{
	gGateLogic->m_pSysNetface->NotifyPlayerOnline(
		gGateLogic->GetIndex(), pUser->m_llUid);
}

void CPlayerProxy::NotifyPlayerOffline(CUser* pUser)
{
	gGateLogic->m_pSysNetface->NotifyPlayerOffline(
		gGateLogic->GetIndex(), pUser->m_llUid);
}

void CPlayerProxy::Handle(CUser* pUser, const tagMsgHead* pNetMsgHead)
{
	//µÇÂ¼GameServerÊ§°Ü
	if (pUser->GetStatus() != CUser::STATUS_CheckedOK)
	{
		Log_Error("not in game !! roleid:%lld, moduleid:%u, cmdid:%u",
			pUser->m_llUid, pNetMsgHead->usModuleId, pNetMsgHead->uiCmdId);
		NotifyErrorCode(pUser, pNetMsgHead->usModuleId, pNetMsgHead->uiCmdId,
			ProtoMsg::Code_Login_NoInGame, pNetMsgHead->uiSeqid);
		return;
	}

	switch (pNetMsgHead->usModuleId)
	{
	case MsgModule::Login:
		HandleLoginModule(pUser, pNetMsgHead);
		break;
	default:
		SendToGameSvr(pUser, pNetMsgHead);
		break;
	}
}

void CPlayerProxy::HandleLoginModule(CUser* pUser, const tagMsgHead* pNetMsgHead)
{
	switch (pNetMsgHead->uiCmdId)
	{
	case MsgModule_Login::Msg_Login_CreateUser_Req:
		OnCreatePlayer(pUser, pNetMsgHead);
		break;
	default:
		SendToGameSvr(pUser, pNetMsgHead);
		break;
	}
}

void CPlayerProxy::SendToGameSvr(CUser* pUser, const tagMsgHead* pNetMsgHead)
{
	//µÇÂ¼GameServerÊ§°Ü
	if (pUser->GetStatus() != CUser::STATUS_CheckedOK || pUser->m_llUid == 0)
	{
		Log_Error("user not in game !! playerid:%lld, moduleid:%u, cmdid:%u",
			pUser->m_llUid, pNetMsgHead->usModuleId, pNetMsgHead->uiCmdId);
		NotifyErrorCode(pUser, pNetMsgHead->usModuleId, pNetMsgHead->uiCmdId,
			ProtoMsg::Code_Login_NoInGame, pNetMsgHead->uiSeqid);
		return;
	}
	TransmitToServer(gGateLogic->m_pGameNetface, pUser, pNetMsgHead);

	//GATE_LOGIC_INS->m_pGameNetface->TransmitToGameSvr(pUser, pNetMsgHead);
}

void CPlayerProxy::OnCreatePlayer(CUser* pUser, const tagMsgHead* pNetMsgHead)
{
	Msg_Login_CreateUser_Req CreateMsg;
	PARSE_PTL_HEAD(CreateMsg, pNetMsgHead);

	const std::string& strName = CreateMsg.strusername();
	ResultCode eCode = ResultCode::Code_Common_Success;
	if (strName.length() > MAX_PALYER_NAME_LEN)
		eCode = Code_Login_NameTooLong;
	else if (!check_name_valid(strName))
		eCode = Code_Login_NameNotRight;
	
	if (eCode != Code_Common_Success)
	{
		Log_Custom("enter", "player name is invalid! account id:%lld, name:%s"
			, pUser->m_llUid, strName.c_str());

		Msg_Login_VerifyAccount_Rsp oVerifyRsp;
		oVerifyRsp.set_lluserid(pUser->m_llUid);
		oVerifyRsp.set_iflag(eCode);
		oVerifyRsp.set_estatus(EPlayerStatus::ePlayer_Offline);
		pUser->SendToClient(&oVerifyRsp, MsgModule::Login,
			MsgModule_Login::Msg_Login_VerifyAccount_Rsp, pNetMsgHead->uiSeqid, eCode);
		return;
	}

	pUser->m_strCreateName = strName;
	Log_Info("player name = %s, account id=%lld", strName.c_str(), pUser->m_llUid);
	Msg_ServerInner_GG_Create_Req oCreateReq;
	oCreateReq.set_lluserid(pUser->m_llUid);
	oCreateReq.set_uigender(CreateMsg.uigender());
	oCreateReq.set_strusername(strName);
	oCreateReq.set_uiiconid(CreateMsg.uiiconid());
	oCreateReq.set_uiseqid(pNetMsgHead->uiSeqid);
	gGateLogic->m_pGameNetface->Send_Msg(&oCreateReq,
		MsgModule_ServerInner::Msg_ServerInner_GG_Create_Req, MsgModule::ServerInner);
}

void CPlayerProxy::OnRollName(CUser* pUser, const tagMsgHead* pNetMsgHead)
{
	return;
}

void CPlayerProxy::NotifyErrorCode(CUser* pUser, msg_id_t cModuleId, msg_id_t cCmdId
	, ProtoMsg::ResultCode eCode, uint32_t uiReqId)
{
	Msg_Login_Error_Notify stErrorNotify;

	stErrorNotify.set_uimoduleid(cModuleId);
	stErrorNotify.set_uicmdid(cCmdId);
	stErrorNotify.set_eerrorcode(eCode);

	pUser->SendToClient(&stErrorNotify, ProtoMsg::Login,
		MsgModule_Login::Msg_Login_Error_Notify, uiReqId, eCode);
}

void CPlayerProxy::TransmitToServer(svr_reconn* pConn, CUser* pUser, const tagMsgHead* pMsg)
{
	if (!pUser) return;

	Msg_ServerCommon_Transmit oTransmit;
	oTransmit.set_lluid(pUser->m_llUid);
	oTransmit.set_uimoduleid(pMsg->usModuleId);
	oTransmit.set_uicmd(pMsg->uiCmdId);
	oTransmit.set_uisequence(pMsg->uiSeqid);
	oTransmit.set_uiretcode(pMsg->uiCode);
	oTransmit.set_strcmdmsg(NET_DATA_BUF(pMsg), NET_DATA_SIZE(pMsg));
	pConn->Send_Msg(&oTransmit, MsgModule_ServerCommon::Msg_ServerCommon_Transmit);
}

void CPlayerProxy::TransimitToClient(const tagMsgHead* pMsg)
{
	Msg_ServerCommon_Transmit oTransmit;
	PARSE_PTL_HEAD(oTransmit, pMsg);

	static CMsgMake oMakeMsg;
	tagMsgHead* pNetMsgHead = oMakeMsg.MakeHeadMsg(oTransmit.strcmdmsg(),
		oTransmit.uimoduleid(), oTransmit.uicmd(), oTransmit.uisequence(),
		(ProtoMsg::ResultCode)oTransmit.uiretcode());
	if (pNetMsgHead == nullptr)
	{
		Log_Error("module id:%u, proto id:%u", oTransmit.uimoduleid(), oTransmit.uicmd());
		return;
	}

	if (oTransmit.lluid() > 0)
	{
		gGateLogic->m_oUserMgr.SendToPlayer(oTransmit.lluid(), pNetMsgHead);
	}
	else
	{
		if (oTransmit.vecuids_size() > 0)
		{
			for (auto& value : oTransmit.vecuids())
				gGateLogic->m_oUserMgr.SendToPlayer(value, pNetMsgHead);
		}
		else
		{
			gGateLogic->m_oUserMgr.SendToAllPlayer(pNetMsgHead);
		}
	}
}

