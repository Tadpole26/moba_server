#include "CServerNetface.h"
#include "CLogLogic.h"
#include "parse_pb.h"
#include "msg_module_servercommon.pb.h"
#include "global_define.h"
#include "CServerSession.h"
using namespace ProtoMsg;

CClientLogic::CClientLogic()
{
}

CClientLogic::~CClientLogic()
{
}

void CClientLogic::handle_logic_msg(const tagNetMsg* pNetMsg)
{
	const tagHostHd& hd = pNetMsg->m_hd;
	tagMsgHead* pMsgHead = NET_HEAD_MSG(pNetMsg->m_body);

	switch (pMsgHead->usModuleId)
	{
	case MsgModule::ServerCommon:
	{
		HandleServerCommon(pMsgHead, hd);
	}
	break;
	default:
	{
		CSvrLogicFace::handle_logic_msg(pNetMsg);
	}
	break;
	}
}

void CClientLogic::HandleServerCommon(tagMsgHead* pMsgHead, const tagHostHd& hd)
{
	switch (pMsgHead->uiCmdId)
	{
	case MsgModule_ServerCommon::Msg_ServerCommon_RegisterReq:
	{
		OnRegisterServer(pMsgHead, hd);
	}
	break;
	case MsgModule_ServerCommon::Msg_ServerCommon_BeatHartReq:
	{
		if (!has_session(hd)) return;

		Msg_ServerCommon_BeatHartRsp oBeatRsp;
		tagMsgHead* pNetMsgHead = MakeHeadMsg(&oBeatRsp, MsgModule::ServerCommon,
			MsgModule_ServerCommon::Msg_ServerCommon_BeatHartRsp, 0);
		if (pNetMsgHead != nullptr)
			my_send_conn_msg(m_pDispatcher, hd.m_threadOid, hd.m_connOid, pNetMsgHead);
	}
	break;
	default:
	break;
	}
}

void CClientLogic::OnRegisterServer(tagMsgHead* pMsgHead, const tagHostHd& hd)
{
	Msg_ServerCommon_RegisterReq oRegisterReq;
	PARSE_PTL_HEAD(oRegisterReq, pMsgHead);

	CServerSession* pServer = g_SvrMgr.GetServer(oRegisterReq.uiserverkind(), oRegisterReq.uiserverid());
	if (pServer)
	{
		Log_Warning("server kind:%u, server id:%u already connected!"
			, oRegisterReq.uiserverkind(), oRegisterReq.uiserverid());
		return;
	}

	pServer = g_SvrMgr.AddServer(oRegisterReq.uiserverkind(), oRegisterReq.uiserverid());
	if (pServer)
	{
		add_session(pServer, hd, MAX_SIZE_512M);
		Log_Info("accept server id:%u, kind:%d succeed!", oRegisterReq.uiserverid(), oRegisterReq.uiserverkind());

		Msg_ServerCommon_RegisterRsp oRegiserRsp;
		oRegiserRsp.set_uiserverid(LOG_LOGIC_INS->m_oConstCfg.m_uiGroupId);
		oRegiserRsp.set_uiserverkind(SERVER_KIND_LOG);
		pServer->Send_Msg(&oRegiserRsp, MsgModule_ServerCommon::Msg_ServerCommon_RegisterRsp);
	}
}
