#include "CServerNetface.h"
#include "CGameNetface.h"
#include "msg_module.pb.h"
#include "log_mgr.h"
#include "CUser.h"
#include "msg_module_login.pb.h"
#include "parse_pb.h"
#include "CGateLogic.h"

////////////////////////////////////////////////////////////////////////////////////////////////

CClientLogic::CClientLogic() {}

CClientLogic::~CClientLogic() {}

void CClientLogic::handle_logic_msg(const tagNetMsg* pNetMsg)
{
	tagMsgHead* pMsgHead = NET_HEAD_MSG(pNetMsg->m_body);
	if (pMsgHead == nullptr) return;

	//�ͻ��˵�¼��һ����Ϣ(����player����player�б���)
	if (pMsgHead->usModuleId == ProtoMsg::MsgModule::Login && pMsgHead->uiCmdId == ProtoMsg::MsgModule_Login::Msg_Login_VerifyAccount_Req)
	{
		OnVerifyAccount(pMsgHead, pNetMsg->m_hd);
		return;
	}
	//��ͨ���ݰ�,�Ҷ���Ӧ��client��bufferevent
	else if (pMsgHead->usModuleId > ProtoMsg::MsgModule::Begin && pMsgHead->usModuleId < ProtoMsg::MsgModule::End)
		CSvrLogicFace::handle_logic_msg(pNetMsg);
	else
		Log_Error("undefined module:%u!", pMsgHead->usModuleId);

}

//��ͻ��˶Ͽ�����
void CClientLogic::OnDisconnect(CUser* pUser)
{
	kill_session(pUser);
}

void CClientLogic::OnKickConnect(CUser* pUser, bool bSysKill)
{
	if (bSysKill)
		pUser->UpdateStatus(CUser::eUserStatus::STATUS_SysKicking);
	else
		pUser->UpdateStatus(CUser::eUserStatus::STATUS_ReKicking);
	kill_session(pUser);
}

//��¼��֤
void CClientLogic::OnVerifyAccount(const tagMsgHead* pHeadMsg, const tagHostHd& hd)
{
	Msg_Login_VerifyAccount_Req oLoginReq;
	PARSE_PTL_HEAD(oLoginReq, pHeadMsg);

	int64 llUserId = oLoginReq.lluserid();
	if (llUserId == 0)
	{
		send_io_conn_msg(hd, cs_layer_kill, 0, 0);
		Log_Warning("user id is 0 !, name:%s, id:%lld", oLoginReq.strusername().c_str(), llUserId);
		return;
	}

	//�ظ���¼,����
	CLogicSession* pSession = get_session(hd);
	if (pSession != nullptr)
	{
		Log_Warning("user login again! name:%s, id:%lld", oLoginReq.strusername().c_str(), llUserId);
		return;
	}

	//�Ѿ��ڵȴ���������
	Log_Custom("enter", "name:%s, id:%lld", oLoginReq.strusername().c_str(), llUserId);

	//���ڼ�����Ϸ���ݵ����,��ֹɱ�����¼���,��ֹ��ˢ���޼������ݿ�
	if (gGateLogic->m_oUserMgr.GetWaitCheckPlayer(llUserId) != nullptr ||
		gGateLogic->m_oUserMgr.GetCheckingPlayer(llUserId) != nullptr ||
		gGateLogic->m_oUserMgr.GetCheckingPwdPlayer(llUserId) != nullptr)
	{
		//���ڼ�����Ϸ��ֱ��ɱ��
		send_io_conn_msg(hd, cs_layer_kill, 0, 0);
		Log_Warning("already in checking! name:%s, id:%lld", oLoginReq.strusername().c_str(), llUserId);
		return;
	}

	bool bReKick = false;
	//�Ѿ���¼��ֱ��������
	CUser* pUser = gGateLogic->m_oUserMgr.GetInGamePlayer(llUserId);
	if (pUser != nullptr)
	{
		time_t llCurTime = GetCurrTime();
		//����3s��Ƶ��������,ֱ�ӷ���
		if (pUser->m_stStat.m_llRecTime <= 0 || (llCurTime < (pUser->m_stStat.m_llRecTime + 3)))
		{
			send_io_conn_msg(hd, cs_layer_kill, 0, 0);
			Log_Warning("operation more 3 seconds!, name:%s, id:%lld", oLoginReq.strusername().c_str(), llUserId);
			return;
		}

		//������Ϸ�е����������
		//Msg_Login_KickRole_Notify stKickNotify;
		//stKickNotify.set_llaccid(llUserId);
		//stKickNotify.set_straccname(oLoginReq.strusername());
		//stKickNotify.set_iflag(ResultCode::Code_Login_RoleInGame);
		//stKickNotify.set_uigameid(pUser->GetGameId());
		//pUser->SendToClient(&stKickNotify, ProtoMsg::Login, MsgModule_Login::Msg_Login_KickRole_Notify,
		//	0, ResultCode::Code_Login_RoleInGame);
		Log_Warning("kick last connection! name:%s, id:%lld", oLoginReq.strusername().c_str(), llUserId);
		OnKickConnect(pUser, false);
		bReKick = true;
	}
	//�����ŶӶ���
	pUser = gGateLogic->m_oUserMgr.AddNewWaitCheckPlayer(llUserId);
	if (pUser == nullptr) return;

	pUser->m_bReKick = bReKick;
	pUser->m_strAccName = oLoginReq.strusername();
	pUser->m_uiSequence = pHeadMsg->uiSeqid;
	pUser->m_strSessionId = oLoginReq.strpassword();
	pUser->m_llNextLineUpPushTime = GetCurrTime();

	add_session(pUser, hd, 0);

	if (bReKick || pUser->m_uiReLogin)
	{
		//�����ߵ���Ҷ�������,���߻��豸��,���������Ŷӽ���Ϸ
		gGateLogic->m_oUserMgr.ReLoginUser(pUser);
	}
	else
	{
		gGateLogic->m_oUserMgr.AddWaitCheckPlayer(pUser);
	}
}