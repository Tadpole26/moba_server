#include "CSysNetface.h"
#include "msg_module_servercommon.pb.h"
#include "msg_module_serverinner.pb.h"
#include "parse_pb.h"
#include "log_mgr.h"
#include "CGameLogic.h"
using namespace ProtoMsg;

CSysNetface::CSysNetface()
{
}

CSysNetface::~CSysNetface()
{
}

void CSysNetface::trigger()
{
	Msg_ServerCommon_BeatHart_Req oBeatReq;
	Send_Msg(&oBeatReq, MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Req, MsgModule::ServerCommon);
}

void CSysNetface::SendWChatMsg(const std::string& strClient
	, const std::string& strServer, const std::string& strMsg)
{
}

void CSysNetface::ReqUniqueId(int64 llPlayerId, const std::string& strPlayerName, uint32 uiServerId)
{
	Msg_ServerInner_GS_UniqueIdReq oReq;
	oReq.set_llplayerid(llPlayerId);
	oReq.set_strplayername(strPlayerName);
	oReq.set_uiserverid(uiServerId);
	Send_Msg(&oReq, MsgModule_ServerInner::Msg_ServerInner_GS_UniqueIdReq, MsgModule::ServerInner);
}

void CSysNetface::ModifyPlayerName(int64 llPlayerId, const std::string& strPlayerName
	, uint32 uiServerId)
{
	Msg_ServerInner_GS_ModifyNameReq oReq;
	oReq.set_llplayerid(llPlayerId);
	oReq.set_strplayername(strPlayerName);
	oReq.set_uiserverid(uiServerId);
	oReq.set_uiareano(gGameLogic->m_oConstCfg.m_uiGroupId);
	Send_Msg(&oReq, MsgModule_ServerInner::Msg_ServerInner_GS_ModifyNameReq, MsgModule::ServerInner);
}

void CSysNetface::handle_msg(const tagMsgHead* pNetMsg)
{
	if (nullptr == pNetMsg) return;

	if (pNetMsg->usModuleId == MsgModule::ServerCommon)
	{
		switch (pNetMsg->uiCmdId)
		{
		case MsgModule_ServerCommon::Msg_ServerCommon_Register_Rsp:
		{
			Msg_ServerCommon_Register_Rsp oRegisterRsp;
			PARSE_PTL_HEAD(oRegisterRsp, pNetMsg);

			//gGameLogic->SetServerState(oRsp.uiserverkind(), 1);

			Log_Info("connect sys server id:%u, kind:%d succeed!",
				oRegisterRsp.uiserverid(), oRegisterRsp.uiserverkind());
		}
		break;
		case MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Rsp:
		break;
		default:
			Log_Error("Unknown assistant cmd: %u!", pNetMsg->uiCmdId);
		break;
		}
	}
	else if (pNetMsg->usModuleId == MsgModule::ServerInner)
	{
		switch (pNetMsg->uiCmdId)
		{
		case MsgModule_ServerInner::Msg_ServerInner_GS_UniqueIdRsp:
		{
			Msg_ServerInner_GS_UniqueIdRsp oRsp;
			PARSE_PTL_HEAD(oRsp, pNetMsg);

			Log_Info("server innser unique id rsp");
		}
		break;
		default:
			Log_Error("Unknown assistant cmd: %u!", pNetMsg->uiCmdId);
			break;
		}
	}
}

void CSysNetface::on_connect()
{
	//socket连接成功回调(这里serverid和groupid区分是针对mmo, 一个groupid对应多个serverid)
	Msg_ServerCommon_Register_Req oRegisterReq;
	oRegisterReq.set_uiserverid(GetServerID());
	oRegisterReq.set_uiserverkind(SERVER_KIND_GAME);
	oRegisterReq.set_uiplatid(gGameLogic->m_oConstCfg.m_uiPlatId);
	oRegisterReq.set_uigroupid(gGameLogic->m_oConstCfg.m_uiGroupId);
	Send_Msg(&oRegisterReq, MsgModule_ServerCommon::Msg_ServerCommon_Register_Req, MsgModule::ServerCommon);
}

void CSysNetface::on_disconnect()
{
	Log_Warning("sys server discoonnect!");
}