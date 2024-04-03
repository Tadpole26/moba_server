#include "CUser.h"
#include "msg_module_servercommon.pb.h"
#include "msg_module_serverinner.pb.h"
#include "msg_module_serverdb.pb.h"
#include "CGameLogic.h"
using namespace ProtoMsg;

CUser::CUser()
{
}

CUser::~CUser()
{
}

void CUser::Init()
{
	m_iSequence = 0;
	m_bOnline = false;
}

void CUser::Release()
{
}

void CUser::SetUserData(const ProtoMsg::user_pb_data_t& oUserPbData)
{
	m_oBaseInfoPkg.ParseBaseInfoFromDB(this, oUserPbData);
}

bool CUser::SetNewUserData()
{
	m_oBaseInfoPkg.SetOwnerUser(this);

	return true;
}

void CUser::EnterGame(bool bRelogin)
{
}

void CUser::LeaveGame(bool bSelfLeave)
{
}

bool CUser::SendMsgClient(google::protobuf::Message* pMsg, uint8_t usModuleId, uint32 uiCmd,
	ProtoMsg::ResultCode eCode, bool bNotify)
{
	if (m_bOnline == false && pMsg == nullptr)
		return false;

	Msg_ServerCommon_Transmit oTransmit;
	oTransmit.set_lluid(GetUserId());
	oTransmit.set_uimoduleid(usModuleId);
	oTransmit.set_strcmdmsg(pMsg->SerializeAsString());
	oTransmit.set_uicmd(uiCmd);
	oTransmit.set_uiretcode(eCode);
	oTransmit.set_uisequence(bNotify ? 0 : GetSequence());
	return gGameLogic->m_pGateSession->Send_Msg(&oTransmit, MsgModule_ServerCommon::Msg_ServerCommon_Transmit);
}

bool CUser::SendMsgClient(const std::string& strMsg, uint8_t usModuleId, uint32 uiCmd,
	ProtoMsg::ResultCode eCode, bool bNotify)
{
	if (m_bOnline == false)
		return false;

	Msg_ServerCommon_Transmit oTransmit;
	oTransmit.set_lluid(GetUserId());
	oTransmit.set_uimoduleid(usModuleId);
	oTransmit.set_strcmdmsg(strMsg);
	oTransmit.set_uicmd(uiCmd);
	oTransmit.set_uiretcode(eCode);
	oTransmit.set_uisequence(bNotify ? 0 : GetSequence());
	return gGameLogic->m_pGateSession->Send_Msg(&oTransmit, MsgModule_ServerCommon::Msg_ServerCommon_Transmit);
}

void CUser::SendKickUserNotify(ResultCode eCode)
{
	Msg_ServerInner_GG_Kick_Notify oNotify;
	oNotify.set_lluserid(GetUserId());
	oNotify.set_uiseqid(GetSequence());
	oNotify.set_ecode(eCode);
	gGameLogic->m_pGateSession->Send_Msg(&oNotify, MsgModule_ServerInner::Msg_ServerInner_GG_Kick_Notify, ServerInner);
}

void CUser::SendLoginRsp(ResultCode eCode)
{
	Msg_ServerInner_GG_Login_Rsp oLoginRsp;
	oLoginRsp.set_lluserid(GetUserId());
	oLoginRsp.set_eflag(eCode);
	oLoginRsp.set_iseqid(GetSequence());
	oLoginRsp.set_brequest(false);
	oLoginRsp.set_llopendate(gGameLogic->m_oConstCfg.m_llOpenTime);
	oLoginRsp.set_strusername(GetUserNick());
	oLoginRsp.set_estate(ePlayer_Online);
	gGameLogic->m_pGateSession->Send_Msg(&oLoginRsp, MsgModule_ServerInner::Msg_ServerInner_GG_Login_Rsp, ServerInner);
}

void CUser::SendCreateRsp(ResultCode eCode)
{
	Msg_ServerInner_GG_Create_Rsp oCreateRsp;
	oCreateRsp.set_lluserid(GetUserId());
	oCreateRsp.set_strusername(GetUserNick());
	oCreateRsp.set_ilevel(GetUserLevel());
	oCreateRsp.set_llcreatetime(GetCreateTime());
	oCreateRsp.set_ecode(eCode);
	oCreateRsp.set_iseqid(GetSequence());
	gGameLogic->m_pGateSession->Send_Msg(&oCreateRsp, MsgModule_ServerInner::Msg_ServerInner_GG_Create_Rsp, ServerInner);
}

bool CUser::SendGetUserInfoToDB()
{
	Msg_ServerDB_GD_GetUserInfo_Req oGetReq;
	oGetReq.set_lluserid(GetUserId());
	return gGameLogic->m_pDBNetFace->Send_Msg(&oGetReq, MsgModule_ServerDB::Msg_ServerDB_GD_GetUserInfo_Req, ServerDB);

}

bool CUser::SendCreateUserToDB()
{
	Msg_ServerDB_GD_CreateUser_Req oCreateReq;
	oCreateReq.set_lluserid(GetUserId());

	ServerDB_BaseInfo* pUserBaseInfo = oCreateReq.mutable_ouserinfo();
	if (!pUserBaseInfo) return false;
	pUserBaseInfo->set_lluserid(GetUserId());
	pUserBaseInfo->set_straccount(GetAccount());
	pUserBaseInfo->set_iuserlevel(GetUserLevel());
	pUserBaseInfo->set_igender(GetUserGender());

	return gGameLogic->m_pDBNetFace->Send_Msg(&oCreateReq, MsgModule_ServerDB::Msg_ServerDB_GD_CreateUser_Req, ServerDB);
}