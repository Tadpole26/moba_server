#include "CSession.h"
#include "log_mgr.h"
#include "CUserManager.h"
#include "msg_module_serverinner.pb.h"
#include "parse_pb.h"
#include "util_file.h"
#include "SysHttpFunc.h"
#include "CSysLogic.h"
#include "CDbInstance.h"

ServerPool<CSession> g_SvrMgr;

CSession::CSession()
{
}

CSession::~CSession()
{
}


void CSession::on_disconnect()
{
	Log_Warning("Server kind:%u, id:%u disconnect!", m_eServerKind, m_uiServerId);
	if (SERVER_KIND::SERVER_KIND_GATE == m_eServerKind)
	{
		OnWMsg("Gate", "Sys", "disconnect");
		SYS_PLAYER_MGR_INS->DelUserByGateId(m_uiServerId);
	}
	else if (SERVER_KIND::SERVER_KIND_GAME == m_eServerKind)
	{
		OnWMsg("Game", "Sys", "disconnect");
		SYS_PLAYER_MGR_INS->DelUserByGateId(m_uiServerId);
	}
	g_SvrMgr.DelServer(m_eServerKind, m_uiServerId);
}

void CSession::handle_msg(const tagMsgHead* pNetMsg)
{
	switch (pNetMsg->usModuleId)
	{
	case MsgModule::ServerInner:
	{
		HandleInnerModule(pNetMsg);
	}
	break;
	default:
	{
		Log_Error("undefined module %u!", pNetMsg->usModuleId);
	}
	break;
	}
}

void CSession::HandleInnerModule(const tagMsgHead* pMsgHead)
{
	if (nullptr == pMsgHead) return;

	switch (pMsgHead->uiCmdId)
	{
	case MsgModule_ServerInner::Msg_ServerInner_GS_RecordOnline:
		OnRecordOnlineNum(pMsgHead);
		break;
	case MsgModule_ServerInner::Msg_ServerInner_GS_ClearOnline:
		OnClearOnlineNum(pMsgHead);
		break;
	case MsgModule_ServerInner::Msg_ServerInner_GS_NotifyOnline:
		OnNotifyOnline(pMsgHead);
		break;
	case MsgModule_ServerInner::Msg_ServerInner_GS_NotifyLogout:
		OnNotifyLogout(pMsgHead);
		break;
	case MsgModule_ServerInner::Msg_ServerInner_GS_PlayerLoginReq:
		OnPlayerLogin(pMsgHead);
		break;
	case MsgModule_ServerInner::Msg_ServerInner_GS_UniqueIdReq:
		OnGetUniqueId(pMsgHead);
		break;
	case MsgModule_ServerInner::Msg_ServerInner_GS_ModifyNameReq:
		OnModifyRoleName(pMsgHead);
		break;
	case MsgModule_ServerInner::Msg_ServerInner_GS_UpdatePlayer:
		OnUpdatePlayerInfo(pMsgHead);
		break;
	case MsgModule_ServerInner::Msg_ServerInner_GS_UpdateRmb:
		OnUpdatePlayerInfoMb(pMsgHead);
		break;
	case MsgModule_ServerInner::Msg_ServerInner_GS_WMsg:
	{
		Msg_ServerInner_GS_WMsg oMsg;
		PARSE_PTL_HEAD(oMsg, pMsgHead);
		OnWMsg(oMsg.strclient(), oMsg.strserver(), oMsg.strmsg());
	}
		break;
	default:
		Log_Error("proto:%u not found", pMsgHead->uiCmdId);
		break;
	}
}

void CSession::OnRecordOnlineNum(const tagMsgHead* pMsgHead)
{
	Msg_ServerInner_GS_RecordOnline oRecord;
	PARSE_PTL_HEAD(oRecord, pMsgHead);

	SYS_HTTP_INS->NotifyOnlineNum(oRecord.uionlinenum()
		, oRecord.uiserverid(), oRecord.uiwaitnum()
		, oRecord.uiareano(), oRecord.strip()
		, oRecord.uiport(), oRecord.uimaxnum(), oRecord.strversion());
}

void CSession::OnClearOnlineNum(const tagMsgHead* pMsgHead)
{
	Msg_ServerInner_GS_ClearOnline oRecord;
	PARSE_PTL_HEAD(oRecord, pMsgHead);

	SYS_HTTP_INS->NotifyClearOnlineNum(oRecord.uiserverid(), oRecord.uiareano());
}

void CSession::OnNotifyOnline(const tagMsgHead* pMsgHead)
{
	Msg_ServerInner_GS_NotifyOnline oRecord;
	PARSE_PTL_HEAD(oRecord, pMsgHead);

	SYS_HTTP_INS->NotifyOnline(oRecord.llplayerid(), oRecord.uiserverid()
		, SYS_LOGIC_INS->m_oConstCfg.m_uiGroupId);
}

void CSession::OnNotifyLogout(const tagMsgHead* pMsgHead)
{
	Msg_ServerInner_GS_NotifyLogout oRecord;
	PARSE_PTL_HEAD(oRecord, pMsgHead);

	SYS_PLAYER_MGR_INS->DelUser(oRecord.llplayerid());
	SYS_HTTP_INS->NotifyLogout(oRecord.llplayerid(), oRecord.uiserverid()
		, SYS_LOGIC_INS->m_oConstCfg.m_uiGroupId);
}

void CSession::OnPlayerLogin(const tagMsgHead* pMsgHead)
{
	Msg_ServerInner_GS_PlayerLoginReq oLogin;
	PARSE_PTL_HEAD(oLogin, pMsgHead);

	int64 llPlayerId = oLogin.llplayerid();
	Log_Custom("login", "userid:%lld, session:%s", llPlayerId, oLogin.strsessionid().c_str());

	ResultCode eCode = ResultCode::Code_Common_Success;
	int iProvince = 0;
	std::string strProvince, strSession, strDev, strChannel, strAccName, strCityCode;
	uint32 uiGmLevel = 0, uiArea = 0;

	Msg_ServerInner_GS_PlayerLoginRsp oRet;
	oRet.set_llplayerid(llPlayerId);
	oRet.set_strsessionid(oLogin.strsessionid());
	oRet.set_uigameid(SYS_LOGIC_INS->m_oConstCfg.m_uiGroupId);

	CUser* pUser = SYS_PLAYER_MGR_INS->GetUser(llPlayerId);
	if (pUser)
	{
		pUser->m_uiNewGateId = GetServerId();
		if (!HandleKickPlayer(pUser))
		{
			eCode = ResultCode::Code_Common_Failure;
			goto ErrorLine;
		}
	}
	else
	{
		pUser = SYS_PLAYER_MGR_INS->AddUser(llPlayerId);
		if (!pUser)
		{
			eCode = ResultCode::Code_Common_Failure;
			goto ErrorLine;
		}
		pUser->m_uiGateId = GetServerId();
		if (!HandleOnlinePlayer(pUser))
		{
			eCode = ResultCode::Code_Common_Failure;
			goto ErrorLine;
		}
	}
	
	//从mongo中获取玩家信息(session)
	/*if (!SYS_DB_INS->GetUserLoginInfo(llPlayerId, iProvince, strProvince
		, strSession, uiGmLevel, strDev, uiArea, strChannel, strAccName, strCityCode))*/
	//{
	//	eCode = ResultCode::Code_Common_Failure;
	//	goto ErrorLine;
	//}

	oRet.set_uiprovince(iProvince);
	oRet.set_strprovincename(strProvince);

	//if (strSession != oLogin.strsessionid() || llPlayerId != oLogin.llplayerid())
	//{
	//	eCode = ResultCode::Code_Common_Failure;
	//	goto ErrorLine;
	//}

	oRet.set_uiprovince(iProvince);
	oRet.set_strprovincename(strProvince);
	oRet.set_uigmlevel(uiGmLevel);
	oRet.set_strdevid(strDev);
	oRet.set_uiproarea(uiArea);
	oRet.set_strchannel(strChannel);
	oRet.set_strplayername(strAccName);
	oRet.set_strcitycode(strCityCode);
ErrorLine:
	if (eCode != ResultCode::Code_Common_Success)
	{
		SYS_PLAYER_MGR_INS->DelUser(oLogin.llplayerid());
	}
	oRet.set_eresult(eCode);
	Send_Msg(&oRet, MsgModule_ServerInner::Msg_ServerInner_GS_PlayerLoginRsp, MsgModule::ServerInner);
}

void  CSession::SendPlayerLoginRet(int64 llPlayerId, ProtoMsg::ResultCode eCode
	, uint32 uiGateId, int iProvince, std::string strProvince
	, std::string strSessionId, uint32 uiGmLevel, std::string strDevId, uint32 uiProArea)
{
	Msg_ServerInner_GS_PlayerLoginRsp oRet;
	oRet.set_llplayerid(llPlayerId);
	oRet.set_uigameid(SYS_LOGIC_INS->m_oConstCfg.m_uiGroupId);
	oRet.set_uiprovince(iProvince);
	oRet.set_strprovincename(strProvince);
	oRet.set_uigmlevel(uiGmLevel);
	oRet.set_strdevid(strDevId);
	oRet.set_uiproarea(uiProArea);

	CUser* pUser = nullptr;
	CSession* pServer = g_SvrMgr.GetServer(SERVER_KIND_GATE, uiGateId);
	if (pServer == nullptr) return;

	if (eCode != ResultCode::Code_Common_Success) goto ErrorLine;

	pUser = SYS_PLAYER_MGR_INS->GetUser(llPlayerId);
	if (pUser)
	{
		pUser->m_uiNewGateId = pServer->GetServerId();
		if (!HandleKickPlayer(pUser))
		{
			eCode = ResultCode::Code_Common_Success;
			goto ErrorLine;
		}
	}
	else
	{
		pUser = SYS_PLAYER_MGR_INS->AddUser(llPlayerId);
		if (!pUser)
		{
			eCode = ResultCode::Code_Common_Failure;
			goto ErrorLine;
		}
		pUser->m_uiGateId = pServer->GetServerId();

		if (!HandleOnlinePlayer(pUser))
		{
			eCode = ResultCode::Code_Common_Failure;
			goto ErrorLine;
		}
	}
	oRet.set_uigameid(pUser->m_uiGameId);
ErrorLine:
	if (eCode != ResultCode::Code_Common_Success)
	{
		SYS_PLAYER_MGR_INS->DelUser(llPlayerId);
	}
	oRet.set_eresult(eCode);
	pServer->Send_Msg(&oRet, MsgModule_ServerInner::Msg_ServerInner_GS_PlayerLoginRsp
		, MsgModule::ServerInner);
}

void CSession::OnGetUniqueId(const tagMsgHead* pMsgHead)
{
	Msg_ServerInner_GS_UniqueIdReq oReq;
	PARSE_PTL_HEAD(oReq, pMsgHead);

	ResultCode eCode = ResultCode::Code_Common_Success;
	//插入mongo
	//int iRet = SYS_DB_INS->InsertNewRole(oReq.llplayerid(), oReq.strplayername());
	//if (-1 == iRet)
	//	eCode = ResultCode::Code_Common_Failure;
	//else if (-2 == iRet)
	//	eCode = ResultCode::Code_Common_Failure;

	Msg_ServerInner_GS_UniqueIdRsp oRsp;
	
	oRsp.set_eret(eCode);
	oRsp.set_llplayerid(oReq.llplayerid());
	oRsp.set_strplayername(oReq.strplayername());
	Send_Msg(&oRsp, MsgModule_ServerInner::Msg_ServerInner_GS_UniqueIdRsp, MsgModule::ServerInner);
	
}

void CSession::OnModifyRoleName(const tagMsgHead* pMsgHead)
{
	Msg_ServerInner_GS_ModifyNameReq oReq;
	PARSE_PTL_HEAD(oReq, pMsgHead);

	ResultCode eCode = ResultCode::Code_Common_Success;
	//mongo 修改名字
	//if (!SYS_DB_INS->ModifyRoleName(oReq.llplayerid(), oReq.strplayername()))
	//	eCode = ResultCode::Code_Common_Failure;

	Msg_ServerInner_GS_ModifyNameRsp oRsp;
	oRsp.set_eret(eCode);
	oRsp.set_llplayerid(oReq.llplayerid());
	oRsp.set_strplayername(oReq.strplayername());
	oRsp.set_bgm(oReq.bgm());
	Send_Msg(&oRsp, MsgModule_ServerInner::Msg_ServerInner_GS_ModifyNameRsp, MsgModule::ServerInner);
}

void CSession::OnUpdatePlayerInfo(const tagMsgHead* pMsgHead)
{
	Msg_ServerInner_GS_UpdatePlayer oUpdate;
	PARSE_PTL_HEAD(oUpdate, pMsgHead);
	//mongo更新数据库
	//SYS_DB_INS->UpdateRoleInfo(oUpdate.oplayer().llplayerid(), oUpdate.oplayer().strplayername()
	//	, oUpdate.oplayer().uilevel(), 1, oUpdate.blogin()
	//	, oUpdate.oplayer().lllogintime(), oUpdate.oplayer().lllogouttime());
}

void CSession::OnUpdatePlayerInfoMb(const tagMsgHead* pMsgHead)
{
	Msg_ServerInner_GS_UpdateRmb oUpdate;
	PARSE_PTL_HEAD(oUpdate, pMsgHead);
	//SYS_DB_INS->UpdateRoleInfoMB(oUpdate.llplayerid(), oUpdate.uirmb(), oUpdate.uirechare());
	//mongo修改rmb
}

bool CSession::HandleOnlinePlayer(CUser* pPlayer)
{
	if (!pPlayer)
	{
		Log_Error("pPlayer is null!");
		return false;
	}
	pPlayer->m_uiGameId = SYS_LOGIC_INS->m_oConstCfg.m_uiGroupId;
	pPlayer->m_eUserStatus = EPlayerStatus::ePlayer_Online;
	return true;
}

bool CSession::HandleKickPlayer(CUser* pPlayer)
{
	if (!pPlayer)
	{
		Log_Error("pPlayer is null");
		return false;
	}

	switch (pPlayer->m_eUserStatus)
	{
	case EPlayerStatus::ePlayer_Online:
	{
		if (!KickPlayerFromGate(pPlayer))
			return false;
		return true;
	}
	break;
	default:
	{
		return false;
	}
	break;
	}
	return true;
}

bool CSession::KickPlayerFromGate(CUser* pPlayer)
{
	if (!pPlayer)
		return false;

	if (pPlayer->m_uiGateId != pPlayer->m_uiNewGateId)
	{
		Msg_ServerInner_GS_KickPlayerNotify oKick;
		oKick.set_llplayerid(pPlayer->m_llUid);
		CSession* pServer = g_SvrMgr.GetServer(SERVER_KIND::SERVER_KIND_GATE, pPlayer->m_uiGateId);
		if (!pServer)
			return false;
		pServer->Send_Msg(&oKick
			, MsgModule_ServerInner::Msg_ServerInner_GS_KickPlayerNotify
			, MsgModule::ServerInner);
	}
	pPlayer->m_uiGateId = pPlayer->m_uiNewGateId;
	pPlayer->m_uiNewGateId = 0;
	return true;
}

void CSession::OnWMsg(const std::string& strClient
	, const std::string& strServer, const std::string& strMsg)
{
	std::stringstream ss;
	ss << ", server: " << get_exec_name();
	ss << ", Arean: " << SYS_LOGIC_INS->m_oConstCfg.m_uiGroupId;
	ss << ", index: " << SYS_LOGIC_INS->m_stArgOpt.GetIndex();
	ss << ", ip: " << SYS_LOGIC_INS->m_strExIp;
	//ss << ", platid: " << SYS_LOGIC_INS->m_stSysConfig.m_uiPlatId;
	ss << ", " << strClient << "->" << strServer;
	ss << ", " << strMsg;

	std::string strErr = ss.str();
	Log_Custom("msg", "%s", strErr.c_str());
	SYS_HTTP_INS->SendChatMsg(strErr);

}

void CSession::SendCreateRoleRet(int64 llPlayerId, uint32 uiGameId, std::string strRoleName)
{
	CSession* pServer = g_SvrMgr.GetServer(SERVER_KIND_GAME, uiGameId);
	if (pServer != nullptr)
	{
		Msg_ServerInner_GS_UniqueIdRsp oRsp;
		oRsp.set_llplayerid(llPlayerId);
		oRsp.set_strplayername(strRoleName);
		pServer->Send_Msg(&oRsp, MsgModule_ServerInner::Msg_ServerInner_GS_UniqueIdRsp, MsgModule::ServerInner);
	}
}

void CSession::SendModifyRoleNameRet(int64 llPlayerId, uint32 uiGameId
	, std::string strRoleName, ProtoMsg::ResultCode eCode)
{
	CSession* pServer = g_SvrMgr.GetServer(SERVER_KIND_GAME, uiGameId);
	if (pServer != nullptr)
	{
		Msg_ServerInner_GS_ModifyNameRsp oRet;
		oRet.set_llplayerid(llPlayerId);
		oRet.set_strplayername(strRoleName);
		oRet.set_eret(eCode);

		pServer->Send_Msg(&oRet, MsgModule_ServerInner::Msg_ServerInner_GS_ModifyNameRsp, MsgModule::ServerInner);
	}
}