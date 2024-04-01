#include "CLogNetface.h"
#include "msg_module_servercommon.pb.h"
#include "msg_module.pb.h"
#include "log_mgr.h"
#include "parse_pb.h"
#include "CBattleLogic.h"

CLogNetFace::CLogNetFace()
{
}

CLogNetFace::~CLogNetFace()
{
}

void CLogNetFace::LogUpLoadFile(int64_t llRoomId, const std::string& strRoomLog, const std::string& strLogPath, size_t uiSize)
{

}

void CLogNetFace::trigger()
{
	Msg_ServerCommon_BeatHart_Req oReq;
	Send_Msg(&oReq, MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Req);
}

void CLogNetFace::handle_msg(const tagMsgHead* pNetMsg)
{
	if (pNetMsg->usModuleId == MsgModule::ServerCommon)
	{
		HandleServerCommon(pNetMsg);
		return;
	}
}

void CLogNetFace::HandleServerCommon(const tagMsgHead* pMsg)
{
	switch (pMsg->uiCmdId)
	{
	case MsgModule_ServerCommon::Msg_ServerCommon_Register_Rsp:
	{
		Msg_ServerCommon_Register_Rsp oRsp;
		PARSE_PTL_HEAD(oRsp, pMsg);

		Log_Info("connect log server id:%u succeed!", oRsp.uiserverid());
	}
	break;
	case MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Rsp:
	{
	}
	break;
	default:
		Log_Error("undefined cmd %u!", pMsg->uiCmdId);
		break;
	}
}

void CLogNetFace::on_connect()
{
	Msg_ServerCommon_Register_Req oReq;
	oReq.set_uiserverkind(SERVER_KIND_BATTLE);
	oReq.set_uiserverid(GetServerID());
	oReq.set_uiplatid(gBattleLogic->GetPlatId());
	oReq.set_uigroupid(gBattleLogic->GetGroupId());

	Send_Msg(&oReq, MsgModule_ServerCommon::Msg_ServerCommon_Register_Req, MsgModule::ServerCommon);
}

void CLogNetFace::on_disconnect()
{
	Log_Warning("log server disconnect!");
}