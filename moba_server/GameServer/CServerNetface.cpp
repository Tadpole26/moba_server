#include "CServerNetface.h"
#include "msg_module_servercommon.pb.h"
#include "msg_module.pb.h"
#include "parse_pb.h"
#include "msg_make.h"
#include "log_mgr.h"
#include "global_define.h"
#include "CGameLogic.h"
#include "CGateSession.h"
using namespace ProtoMsg;

CClientLogic::CClientLogic() { }
CClientLogic::~CClientLogic() { }

void CClientLogic::handle_logic_msg(const tagNetMsg* pNetMsg)
{
	const tagHostHd& stHd = pNetMsg->m_hd;
	tagMsgHead* pMsgHead = (tagMsgHead*)(pNetMsg->m_body);

	switch (pMsgHead->usModuleId)
	{
	case MsgModule::ServerCommon:
	{
		if (pMsgHead->uiCmdId == MsgModule_ServerCommon::Msg_ServerCommon_Register_Req)
		{
			Msg_ServerCommon_Register_Req oRegisterReq;
			PARSE_PTL_HEAD(oRegisterReq, pMsgHead);

			CGateSession* pServer = new CGateSession();
			if (!pServer)
			{
				Log_Warning("gate server id:%u new error!", oRegisterReq.uiserverid());
				return;
			}
			pServer->SetServerKind((SERVER_KIND)oRegisterReq.uiserverkind());
			pServer->SetServerId(oRegisterReq.uiserverid());
			add_session(pServer, pNetMsg->m_hd, MAX_SIZE_512M);
			gGameLogic->m_pGateSession = pServer;
			Log_Info("accept gate server id:%u, kind:%d succeed!" , oRegisterReq.uiserverid(), oRegisterReq.uiserverkind());

			Msg_ServerCommon_Register_Rsp oRegisterRsp;
			oRegisterRsp.set_uiserverid(gGameLogic->m_oConstCfg.m_uiGroupId);
			oRegisterRsp.set_uiserverkind(SERVER_KIND_GAME);
			oRegisterRsp.set_strcomment("default version");
			//游戏服务器是否启动完成(load排行榜、邮件、玩家数据全部完成设置成true)
			oRegisterRsp.set_bfirstcon(true);
			pServer->Send_Msg(&oRegisterRsp, MsgModule_ServerCommon::Msg_ServerCommon_Register_Rsp, MsgModule::ServerCommon);
		}
		else if (pMsgHead->uiCmdId == MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Req)
		{
			if (!has_session(pNetMsg->m_hd)) return;

			Msg_ServerCommon_BeatHart_Req oBeatReq;
			tagMsgHead* pNetMsgHead = MakeHeadMsg(&oBeatReq, MsgModule::ServerCommon,
				MsgModule_ServerCommon::Msg_ServerCommon_BeatHart_Rsp, 0);
			if (pNetMsgHead != nullptr)
				my_send_conn_msg(m_pDispatcher, pNetMsg->m_hd.m_threadOid, pNetMsg->m_hd.m_connOid, pNetMsgHead);
		}
		else if (pMsgHead->uiCmdId == MsgModule_ServerCommon::Msg_ServerCommon_Transmit)
		{
			CGateSession* pSession = (CGateSession*)get_session(pNetMsg->m_hd);
			if (pSession)
				pSession->HandleTransmitData(pMsgHead);
		}
		break;
	}
	default:
	{
		CSvrLogicFace::handle_logic_msg(pNetMsg);
	}
		break;
	}
}