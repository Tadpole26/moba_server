#include "CDBNetface.h"
#include "msg_module_servercommon.pb.h"
#include "msg_module_serverdb.pb.h"
#include "log_mgr.h"
#include "parse_pb.h"
#include "CUserManager.h"
#include "CUser.h"
using namespace ProtoMsg;

CDBNetface::CDBNetface()
{
}

CDBNetface::~CDBNetface()
{
}

void CDBNetface::trigger()
{
	static Msg_ServerCommon_BeatHart_Req oReq;
	Send_Msg(&oReq, MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Req);
}

void CDBNetface::handle_msg(const tagMsgHead* pNetMsg)
{
	if (nullptr == pNetMsg) return;

	if (pNetMsg->usModuleId == MsgModule::ServerCommon)
	{
		HandleServerCommon(pNetMsg);
		return;
	}

	switch (pNetMsg->uiCmdId)
	{
	case MsgModule_ServerDB::Msg_ServerDB_GD_GetUserInfo_Rsp:
		OnGetUserInfo(pNetMsg);
		break;
	case MsgModule_ServerDB::Msg_ServerDB_GD_CreateUser_Rsp:
		OnCreateUser(pNetMsg);
		break;
	default:
		Log_Error("can not find cmd id:%u", pNetMsg->uiCmdId);
		break;
	}
}

void CDBNetface::HandleServerCommon(const tagMsgHead* pMsg)
{
	switch (pMsg->uiCmdId)
	{
	case MsgModule_ServerCommon::Msg_ServerCommon_Register_Rsp:
	{
		Msg_ServerCommon_Register_Rsp oRsp;
		PARSE_PTL_HEAD(oRsp, pMsg);

		Log_Info("connect db server id:%u, kind:%u succeed!", oRsp.uiserverid(), oRsp.uiserverkind());
	}
	break;
	case MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Rsp:
		break;
	default:
		Log_Error("undefined cmd %u", pMsg->uiCmdId);
		break;
	}
}

bool DecodeDBUserData(user_pb_data_t& oUserPbData, const buff_data_t& strData)
{
	PARSE_PTL_STR_RET(oUserPbData, strData.strbuffdata());
	return true;
}

void CDBNetface::OnGetUserInfo(const tagMsgHead* pMsg)
{
	Msg_ServerDB_GD_GetUserInfo_Rsp oGetRsp;
	PARSE_PTL_HEAD(oGetRsp, pMsg);

	CUser* pUser = gUserManager->GetCheckingUser(oGetRsp.lluserid());
	if (pUser == nullptr)
	{
		Log_Error("can not find user id:%lld", oGetRsp.lluserid());
		return;
	}

	user_pb_data_t oUserPbData;
	if (oGetRsp.ecode() == ResultCode::Code_Common_Success)
	{
		if (DecodeDBUserData(oUserPbData, oGetRsp.ouserdata()))
		{
			pUser->SetUserData(oUserPbData);
			pUser->EnterGame();
			pUser->SendLoginRsp();

			gUserManager->MoveCheckedUser(pUser);

			Log_Custom("enter", "get user info, user name:%s, account name:%s, userid:%lld",
				pUser->GetUserNick().c_str(), pUser->GetAccount().c_str(), pUser->GetUserId());
		}
		else
		{
			//解压失败的直接踢掉
			pUser->SendLoginRsp(ResultCode::Code_Common_Failure);
			Log_Error("get user info failed, ret code:%d", oGetRsp.ecode());
			gUserManager->DelCheckingUser(pUser->GetUserId(), true);
		}
	}
	else if (oGetRsp.ecode() == ResultCode::Code_Login_NoUser)
	{
		pUser->SendLoginRsp(oGetRsp.ecode());
		Log_Custom("enter", "no user, account name:%s, userid:%lld", pUser->GetAccount().c_str(), pUser->GetUserId());
	}
	else
	{
		pUser->SendLoginRsp(oGetRsp.ecode());
		Log_Error("get user info failed, ret code:%d", oGetRsp.ecode());
		gUserManager->DelCheckingUser(pUser->GetUserId(), true);
	}
}

void CDBNetface::OnCreateUser(const tagMsgHead* pMsg)
{
	Msg_ServerDB_GD_CreateUser_Rsp oCreateRsp;
	PARSE_PTL_HEAD(oCreateRsp, pMsg);

	CUser* pUser = gUserManager->GetCheckingUser(oCreateRsp.lluserid());
	if (pUser == nullptr)
	{
		Log_Error("can not find user id:%lld", oCreateRsp.lluserid());
		return;
	}

	if (oCreateRsp.ecode() == ResultCode::Code_Common_Success)
	{
		pUser->EnterGame();
		pUser->SendCreateRsp();
		gUserManager->MoveCheckedUser(pUser);
		Log_Custom("create", "create user success, user name=%s, account:%s, userid:%lld",
			pUser->GetUserNick().c_str(), pUser->GetAccount().c_str(), pUser->GetUserId());
	}
	else
	{
		Log_Custom("create", "create user failed, user name=%s, account:%s, userid:%lld",
			pUser->GetUserNick().c_str(), pUser->GetAccount().c_str(), pUser->GetUserId());
	}
}

void CDBNetface::on_disconnect()
{
	Log_Warning("DB server disconnect!");
	gUserManager->KillAllUser();
}

void CDBNetface::on_connect()
{
	Msg_ServerCommon_Register_Req oRegisterReq;
	oRegisterReq.set_uiserverkind(SERVER_KIND_GAME);
	oRegisterReq.set_uiserverid(GetServerID());
	Send_Msg(&oRegisterReq, MsgModule_ServerCommon::Msg_ServerCommon_Register_Req);

	CTcpReconn::on_connect();
}