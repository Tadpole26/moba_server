#include "CCrossNetface.h"
#include "msg_module_servercommon.pb.h"
#include "CGameLogic.h"
#include "parse_pb.h"
using namespace ProtoMsg;

CCrossNetface::CCrossNetface()
{
}

CCrossNetface::~CCrossNetface()
{
}

void CCrossNetface::handle_msg(const tagMsgHead* pNetMsg)
{
	if (nullptr == pNetMsg) return;

	switch (pNetMsg->usModuleId)
	{
	case MsgModule::ServerCommon:
		HandleServerCommon(pNetMsg);
		break;
	case MsgModule::ServerInner:
		HandleCrossModule(pNetMsg);
		break;
	default:
		Log_Error("undefine module %u, cmd:%u", pNetMsg->usModuleId, pNetMsg->uiCmdId);
		break;
	}
}

void CCrossNetface::on_connect()
{
	Msg_ServerCommon_Register_Req oRegisterReq;
	oRegisterReq.set_uiserverid(GetServerID());
	oRegisterReq.set_uiserverkind(SERVER_KIND_GAME);
	//oRegisterReq.set_strexip(gGameLogic->);
	oRegisterReq.set_uiindex(gGameLogic->m_stArgOpt.GetIndex());
	oRegisterReq.set_uiplatid(gGameLogic->m_oConstCfg.m_uiPlatId);
	oRegisterReq.set_uigroupid(gGameLogic->m_oConstCfg.m_uiGroupId);
	//oRegisterReq.set_open
	Send_Msg(&oRegisterReq, MsgModule_ServerCommon::Msg_ServerCommon_Register_Req, MsgModule::ServerCommon);
	CTcpReconn::on_connect();
}

void CCrossNetface::on_disconnect()
{
	Log_Warning("cross server disconnect!");
}

void CCrossNetface::trigger()
{
	static Msg_ServerCommon_BeatHart_Req oBeatReq;
	Send_Msg(&oBeatReq, MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Req, MsgModule::ServerCommon);
}

void CCrossNetface::HandleServerCommon(const tagMsgHead* pMsgHead)
{
	switch (pMsgHead->uiCmdId)
	{
	case MsgModule_ServerCommon::Msg_ServerCommon_Register_Rsp:
	{
		Msg_ServerCommon_Register_Rsp oRegisterRsp;
		PARSE_PTL_HEAD(oRegisterRsp, pMsgHead);
		Log_Info("connect cross server id:%u, kind:%d succeed!",
			oRegisterRsp.uiserverid(), oRegisterRsp.uiserverkind());
	}
	break;
	case MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Req:
		break;
	case MsgModule_ServerCommon::Msg_ServerCommon_Transmit:
		HandleTransmitData(pMsgHead);
		break;
	default :
		Log_Error("undefined cmd %u!, pMsgHead->uiCmdId");
		break;
	}
}

void CCrossNetface::HandleCrossModule(const tagMsgHead* pMsgHead)
{
}

void CCrossNetface::HandleTransmitData(const tagMsgHead* pMsgHead)
{
	Msg_ServerCommon_Transmit oTransmitRsp;
	PARSE_PTL_HEAD(oTransmitRsp, pMsgHead);

	//for (const auto& uid : oTransmitRsp.vecuids())
	//{

	//}
}