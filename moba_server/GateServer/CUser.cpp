#include "CUser.h"
#include "util_time.h"
#include "msg_module_login.pb.h"
#include "msg_module_serverinner.pb.h"
#include "result_code.pb.h"
#include "log_mgr.h"
#include "CGateLogic.h"
#include "CGameNetface.h"
#include "PlayerProxy.h"
using namespace ProtoMsg;
//#include "func_util.h"

CUser::CUser() {}

CUser::~CUser() {}

void CUser::Init()
{
	m_uiReLogin = 0;
	m_bSaveFlag = false;
	m_uiSequence = 0;
	m_uiSdk = 0;
	m_uiGameId = 0;
	m_llNextLineUpPushTime = 0;

	m_bReKick = false;
	m_llUid = 0;
	m_uiProvince = 0;
	m_uiProArea = 0;
}

void CUser::Release()
{
	m_hd.m_threadOid = invalid_thread_oid;
	m_hd.m_connOid = invalid_conn_oid;
	m_llUid = 0;
	m_bSaveFlag = true;
	m_strAccName.clear();
	m_uiSdk = 0;
	m_uiGameId = 0;
	m_strDevId.clear();
	m_stStat.Clear();
	m_strProvince.clear();
	m_oBeginTime.Clear();
	m_strCreateName.clear();
	m_strChannel.clear();
}

void CUser::handle_msg(const tagMsgHead* pNetMsg)
{
	//客户端和服务器之间的心跳包,在网络层收到消息更新时间,网络层长时间没有收到包会将socket断开
	if (pNetMsg->usModuleId == ProtoMsg::MsgModule::Login && pNetMsg->uiCmdId == ProtoMsg::MsgModule_Login::Msg_Login_BeatHeart_Req)
	{
		static ProtoMsg::Msg_Login_BeatHeart_Rsp stMsgRsp;
		stMsgRsp.set_llservertime(GetCurrTime());
		SendToClient(&stMsgRsp, ProtoMsg::MsgModule::Login, 
				ProtoMsg::MsgModule_Login::Msg_Login_BeatHeart_Rsp,
				pNetMsg->uiSeqid, ProtoMsg::ResultCode::Code_Common_Success);
		return;
	}
	CPlayerProxy::Handle(this, pNetMsg);
	return;
}

void CUser::on_disconnect()
{
	if (m_uiGameId != 0)
	{
		Msg_ServerInner_GG_Save_Ntf oSavePlayer;
		oSavePlayer.set_llplayerid(m_llUid);
		oSavePlayer.set_uisaveflag(m_bSaveFlag);
		oSavePlayer.set_strmsg("");
		oSavePlayer.set_uigateid(gGateLogic->m_oConstConfig.m_uiGroupId);
		gGateLogic->m_pGameNetface->Send_Msg(&oSavePlayer,
			MsgModule_ServerInner::Msg_ServerInner_GG_Save_Ntf, MsgModule::ServerInner);
		if (m_stStat.m_eStatus != eUserStatus::STATUS_ReKicking)
			CPlayerProxy::NotifyPlayerOffline(this);
	}
	Log_Custom("enter", "account name:%s, userid %lld, threadid:%u, conid:%u timeout:%d",
		m_strAccName.c_str(), m_llUid, m_hd.m_threadOid, m_hd.m_connOid, IsTimeOut());
	gGateLogic->m_oUserMgr.DelGamePlayer(m_llUid);
}

bool CUser::Send(const tagMsgHead* pMsg)
{
	if (gGateLogic->SetMaxSend(pMsg->uiLen) && pMsg->uiLen > MSG_MAX_LEN / 2)
		Log_Error("module:%u, cmd:%u, size:%u, roleid:%lld", 
			pMsg->usModuleId, pMsg->uiCmdId, pMsg->uiLen, m_llUid);

	return svr_session::Send(pMsg);
}

void CUser::SendToClient(google::protobuf::Message* pMessage, ProtoMsg::MsgModule usModule,
	msg_id_t usProtocol, uint32 uiSequence, ProtoMsg::ResultCode eCode)
{
	Send_Msg(pMessage, usProtocol, usModule, eCode, uiSequence);
}


void CUser::SendToClient(const std::string& strMsg, ProtoMsg::MsgModule usModule,
	msg_id_t usProtocol, uint32 uiSequence, ProtoMsg::ResultCode eCode)
{
	Send_Msg(strMsg, usProtocol, usModule, eCode, uiSequence);
}

bool CUser::UpdateStatus(eUserStatus eStatus)
{
	m_stStat.m_eStatus = eStatus;
	m_stStat.m_llRecTime = GetCurrTime();
	return true;
}

