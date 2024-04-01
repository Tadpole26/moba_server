#include "CCrossNetFace.h"
#include "msg_module_servercommon.pb.h"
#include "msg_module.pb.h"
#include "log_mgr.h"
#include "parse_pb.h"
#include "CBattleLogic.h"
using namespace ProtoMsg;

CCrossNetFace::CCrossNetFace(void) : svr_reconn(true, 180)
{
}

CCrossNetFace::~CCrossNetFace(void)
{
}

void CCrossNetFace::trigger()
{
	static Msg_ServerCommon_BeatHart_Req oBeatReq;
	Send_Msg(&oBeatReq, MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Req, MsgModule::ServerCommon);
}

void CCrossNetFace::on_connect()
{
	CTcpReconn::on_connect();

	Msg_ServerCommon_Register_Req oRegisterReq;
	oRegisterReq.set_strexip(gBattleLogic->GetExIp());
	Send_Msg(&oRegisterReq, MsgModule_ServerCommon::Msg_ServerCommon_Register_Req
	, MsgModule::ServerCommon);
}

void CCrossNetFace::on_disconnect()
{
	Log_Error("cross server disconnect!");
}

void CCrossNetFace::handle_msg(const tagMsgHead* pNetMsg)
{
	if (pNetMsg == nullptr) return;

	if (pNetMsg->usModuleId == MsgModule::ServerInner)
	{
		switch (pNetMsg->uiCmdId)
		{
		case MsgModule_ServerCommon::Msg_ServerCommon_Register_Rsp:
		{
			Msg_ServerCommon_Register_Rsp oRsp;
			PARSE_PTL_HEAD(oRsp, pNetMsg);

			Log_Info("connect cross server id:%u succeed!", oRsp.uiserverid());
		}
		break;
		case  MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Rsp:
		{
		}
		break;
		default:
			Log_Error("undefined cmd:%u!", pNetMsg->uiCmdId);
		break;
		}
	}
}

