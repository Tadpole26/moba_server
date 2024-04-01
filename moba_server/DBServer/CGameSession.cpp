#include "CGameSession.h"
#include "log_mgr.h"
#include "parse_pb.h"
#include "msg_module_serverdb.pb.h"
#include "CDBUser.h"
#include "CDBUserManager.h"
using namespace ProtoMsg;

ServerPool<CGameSession> gSvrManager;

CGameSession::CGameSession()
{
	m_eServerKind = SERVER_KIND_NONE;
	m_uiServerId = 0;
}

CGameSession::~CGameSession()
{
}

void CGameSession::on_disconnect()
{
	Log_Warning("game server %u disconnect!", m_uiServerId);
	gSvrManager.DelServer(m_eServerKind, m_uiServerId);
}

void CGameSession::on_connect()
{
	Log_Custom("start", "connect game server success !!!");
}

void CGameSession::handle_msg(const tagMsgHead* pNetMsg)
{
	unsigned char* pBuf = NET_DATA_BUF(pNetMsg);
	size_t uiLen = NET_DATA_SIZE(pNetMsg);
	switch (pNetMsg->uiCmdId)
	{
	case MsgModule_ServerDB::Msg_ServerDB_GD_GetUserInfo_Req:
		OnGetUserInfo(pBuf, uiLen);
		break;
	case MsgModule_ServerDB::Msg_ServerDB_GD_CreateUser_Req:
		OnCreateUser(pBuf, uiLen);
		break;
	default:
		Log_Error("cmd:%u not found!", pNetMsg->uiCmdId);
		break;
	}
}

bool SetPbFromUser(CDBUser* pUser, buff_data_t* pBuffData)
{
	if (pUser == nullptr || pBuffData == nullptr) return false;

	user_pb_data_t oUserPbData;

	std::string strSrc;
	std::string* pStrByte = pBuffData->mutable_strbuffdata();
	if (pStrByte == nullptr)
		return false;

	if (!SerToString(&oUserPbData, &strSrc))
	{
		Log_Error("serilize user id:%lld error!", pUser->GetUserId());
		return false;
	}

	static size_t uiMsgLen = 0;
	if (uiMsgLen < strSrc.length()) uiMsgLen = strSrc.length();
	Log_Custom("enter", "package user:%lld, maxmsg:%u", pUser->GetUserId(), uiMsgLen);
	*pStrByte = strSrc;
	pBuffData->set_idatalen(strSrc.length());
	return true;
}

void CGameSession::OnGetUserInfo(unsigned char* pMsg, size_t uiLen)
{
	assert(pMsg);
	Msg_ServerDB_GD_GetUserInfo_Req oGetReq;
	PARSE_PTL(oGetReq, pMsg, uiLen);

	Msg_ServerDB_GD_GetUserInfo_Rsp oGetRsp;
	oGetRsp.set_lluserid(oGetReq.lluserid());
	oGetRsp.set_ecode(ResultCode::Code_Common_Success);

	int iResult = 0;
	CDBUser* pUser = gDBUserManager->GetUserOn(oGetReq.lluserid());
	if (pUser == nullptr)
	{
		pUser = gDBUserManager->GetUserOff(oGetReq.lluserid());
		if (pUser != nullptr)
		{
			pUser->SetOnline(true);
			gDBUserManager->MoveToOn(pUser);
		}
		else
			pUser = gDBUserManager->LoadUserOn(oGetReq.lluserid(), iResult);
	}

	if (pUser == nullptr)
	{
		if (iResult == 2)			//没数据,返回客户端走创角流程
		{
			oGetRsp.set_ecode(ResultCode::Code_Login_NoInGame);
			Log_Info("no user, user id:%lld need to create", oGetReq.lluserid());
		}
		else if (iResult == 1)		//数据错误
		{
			oGetRsp.set_ecode(ResultCode::Code_Common_Failure);
			Log_Error("no user, user id:%lld data error!!!", oGetReq.lluserid());
		}
	}
	else
	{
		if (!SetPbFromUser(pUser, oGetRsp.mutable_ouserdata()))
		{
			oGetRsp.set_ecode(ResultCode::Code_Login_NoInGame);
			Log_Info("get user id:%lld data error", oGetReq.lluserid());
		}
		else
		{
			Log_Custom("enter", "load user userid:%lld succeed!", pUser->GetUserId());
		}
	}
	Send_Msg(&oGetRsp, MsgModule_ServerDB::Msg_ServerDB_GD_GetUserInfo_Rsp, MsgModule::ServerDB);
}

void CGameSession::OnCreateUser(unsigned char* pMsg, size_t uiLen)
{
	assert(pMsg);

	Msg_ServerDB_GD_CreateUser_Req oCreateReq;
	PARSE_PTL(oCreateReq, pMsg, uiLen);

	Msg_ServerDB_GD_CreateUser_Rsp oCreateRsp;
	oCreateRsp.set_lluserid(oCreateReq.lluserid());
	oCreateRsp.set_ecode(ResultCode::Code_Common_Success);
}