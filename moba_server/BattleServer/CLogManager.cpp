#include "CLogManager.h"
#include "msg_module_serverinner.pb.h"
#include "CRoomManager.h"
#include "CUserManager.h"
#include "CBattleLogic.h"

CLogManager::CLogManager()
{
}

CLogManager::~CLogManager()
{
}

void CLogManager::SubmitLoad(bool bThread)
{
	ProtoMsg::Msg_ServerInner_BC_LoadInfo_Notify oNotify;
	oNotify.Clear();
	oNotify.set_iroomnum((int)gRoomManager->GetRoomNum());
	oNotify.set_iusernum((int)gUserManager->GetUserNum());

	if (!bThread)
	{
		gBattleLogic->m_pCrossNetFace->Send_Msg(&oNotify, MsgModule_ServerInner::Msg_ServerInner_BC_LoadInfo_Notify, ProtoMsg::ServerInner);
		return;
	}
	SendWorldMsg(oNotify.SerializeAsString(), MsgModule_ServerInner::Msg_ServerInner_BC_LoadInfo_Notify);
}

void CLogManager::SubmitRoomInfo(CRoom* pRoom, bool bThread)
{
	ProtoMsg::Msg_ServerInner_BC_RoomInfo_Notify oNotify;
	oNotify.set_iusernum(pRoom->GetInUserNum());
	oNotify.set_llroomid(pRoom->GetRoomId());
	oNotify.set_estatus(pRoom->GetRoomStatus());
	oNotify.set_strroomlogid(pRoom->GetLogId());

	if (!bThread)
	{
		gBattleLogic->m_pCrossNetFace->Send_Msg(&oNotify, MsgModule_ServerInner::Msg_ServerInner_BC_RoomInfo_Notify, ProtoMsg::ServerInner);
		return;
	}
	SendWorldMsg(oNotify.SerializeAsString(), MsgModule_ServerInner::Msg_ServerInner_BC_RoomInfo_Notify);
}

void CLogManager::SendLogMsg(const std::string& strMsg, int iId)
{
	stMsgItem stItem;
	stItem.m_eSvrKind = SERVER_KIND_LOG;
	stItem.m_strMsg = strMsg;
	stItem.m_iId = iId;

	m_oLockSend.lock();
	m_lstMsg.push_back(stItem);
	m_oLockSend.unlock();
}

void CLogManager::SendWorldMsg(const std::string& strMsg, int iId, bool bThread)
{
	if (!bThread)
	{
		gBattleLogic->m_pCrossNetFace->Send_Msg(strMsg, iId, ProtoMsg::ServerInner);
		return;
	}

	stMsgItem stItem;
	stItem.m_eSvrKind = SERVER_KIND_CROSS;
	stItem.m_strMsg = strMsg;
	stItem.m_iId = iId;

	m_oLockSend.lock();
	m_lstMsg.push_back(stItem);
	m_oLockSend.unlock();
}


void CLogManager::OnTimer(int iTime)
{
	std::list<stMsgItem> lstSend;

	m_oLockSend.lock();
	lstSend.swap(m_lstMsg);
	m_oLockSend.unlock();

	if (lstSend.size() == 0)
		return;

	for (const auto& iter : lstSend)
	{
		switch (iter.m_eSvrKind)
		{
		case SERVER_KIND_LOG:
			gBattleLogic->m_pLogNetface->Send_Msg(iter.m_strMsg, iter.m_iId, ProtoMsg::ServerInner);
			break;
		case SERVER_KIND_CROSS:
			gBattleLogic->m_pCrossNetFace->Send_Msg(iter.m_strMsg, iter.m_iId, ProtoMsg::ServerInner);
			break;
		default:
			Log_Error("send msg svr type error:%d", iter.m_eSvrKind);
			break;
		}
	}
}
