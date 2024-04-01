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

	//客户端登录第一个消息(生成player加入player列表中)
	if (pMsgHead->usModuleId == ProtoMsg::MsgModule::Login && pMsgHead->uiCmdId == ProtoMsg::MsgModule_Login::Msg_Login_VerifyAccount_Req)
	{
		OnVerifyAccount(pMsgHead, pNetMsg->m_hd);
		return;
	}
	//普通数据包,找都对应的client的bufferevent
	else if (pMsgHead->usModuleId > ProtoMsg::MsgModule::Begin && pMsgHead->usModuleId < ProtoMsg::MsgModule::End)
		CSvrLogicFace::handle_logic_msg(pNetMsg);
	else
		Log_Error("undefined module:%u!", pMsgHead->usModuleId);

}

//与客户端断开连接
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

//登录验证
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

	//重复登录,忽略
	CLogicSession* pSession = get_session(hd);
	if (pSession != nullptr)
	{
		Log_Warning("user login again! name:%s, id:%lld", oLoginReq.strusername().c_str(), llUserId);
		return;
	}

	//已经在等待检测队列中
	Log_Custom("enter", "name:%s, id:%lld", oLoginReq.strusername().c_str(), llUserId);

	//正在加载游戏数据的玩家,禁止杀掉重新加载,防止被刷无限加载数据库
	if (gGateLogic->m_oUserMgr.GetWaitCheckPlayer(llUserId) != nullptr ||
		gGateLogic->m_oUserMgr.GetCheckingPlayer(llUserId) != nullptr ||
		gGateLogic->m_oUserMgr.GetCheckingPwdPlayer(llUserId) != nullptr)
	{
		//正在加载游戏的直接杀掉
		send_io_conn_msg(hd, cs_layer_kill, 0, 0);
		Log_Warning("already in checking! name:%s, id:%lld", oLoginReq.strusername().c_str(), llUserId);
		return;
	}

	bool bReKick = false;
	//已经登录的直接踢下线
	CUser* pUser = gGateLogic->m_oUserMgr.GetInGamePlayer(llUserId);
	if (pUser != nullptr)
	{
		time_t llCurTime = GetCurrTime();
		//超过3s过频操作保护,直接返回
		if (pUser->m_stStat.m_llRecTime <= 0 || (llCurTime < (pUser->m_stStat.m_llRecTime + 3)))
		{
			send_io_conn_msg(hd, cs_layer_kill, 0, 0);
			Log_Warning("operation more 3 seconds!, name:%s, id:%lld", oLoginReq.strusername().c_str(), llUserId);
			return;
		}

		//正在游戏中的玩家踢下线
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
	//加入排队队列
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
		//正在线的玩家断线重连,或者换设备的,可以跳过排队进游戏
		gGateLogic->m_oUserMgr.ReLoginUser(pUser);
	}
	else
	{
		gGateLogic->m_oUserMgr.AddWaitCheckPlayer(pUser);
	}
}