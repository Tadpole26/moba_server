#include "CRoom.h"
#include "msg_module.pb.h"
#include "log_mgr.h"
#include "msg_module_serverbattle.pb.h"
#include "CBattleSession.h"
#include "CBCenterLogic.h"
using namespace ProtoMsg;

CRoom::CRoom()
{
}

CRoom::~CRoom()
{
}

void CRoom::Init()
{
	m_llRoomId = 0;
	m_eRoomType = ProtoMsg::ERoomType_None;
	m_iInGameNum = 0;
	m_bFinished = false;
	m_iCurTime = 0;								//累计时间

	m_iRoomPeoNum = 0;							//房间人数
	m_iCrossSessionId = 0;
	m_eCrossKind = SERVER_KIND_NONE;
	m_iBattleSvrId = 0;
	m_iPort = 0;
	m_eRoomState = ProtoMsg::ERoomStatus_None;
	m_iGroupId = 0;
}

bool CRoom::Init(ProtoMsg::Msg_ServerBattle_CB_CreateRoom_Req& oCreateReq)
{
	m_llRoomId = oCreateReq.llroomid();
	m_strLogId = oCreateReq.strroomlogid();
	m_eRoomType = oCreateReq.etype();
	m_stMapInfo.m_iMapId = oCreateReq.imapid();
	m_stMapInfo.m_strMapName = oCreateReq.strmapname();
	m_stMapInfo.m_iSecond = oCreateReq.imapsec();
	m_iRoomPeoNum = oCreateReq.vecuserdata_size();
	m_iUserNum = oCreateReq.iusernum();
	m_iRobotNum = oCreateReq.irobotnum();
	
	return true;
}

void CRoom::Release()
{
	m_strLogId.clear();
	m_strIp.clear();
	m_stMapInfo.Clear();

	m_llRoomId = 0;
	m_eRoomType = ProtoMsg::ERoomType_None;
	m_iInGameNum = 0;

	m_bFinished = false;
	m_iCurTime = 0;								//累计时间

	m_iRoomPeoNum = 0;							//房间人数
	m_iCrossSessionId = 0;
	m_eCrossKind = SERVER_KIND_NONE;
	m_iBattleSvrId = 0;
	m_iPort = 0;
	m_eRoomState = ProtoMsg::ERoomStatus_None;
	m_iGroupId = 0;
	m_strCreateProto.clear();
}

void CRoom::ClearRoom()
{
	m_bFinished = true;
}

void CRoom::OnTimer(int iTime)
{
	m_iCurTime += iTime;
	if (m_bFinished)
		return;

	if (ProtoMsg::ERoomStatus_End == m_eRoomState || IsTimeOut())
	{
		DoOnverProcess();
		m_bFinished = true;
	}
}

void CRoom::SetRoomState(ProtoMsg::ERoomStatus eState)
{
	m_eRoomState = eState;
}

bool CRoom::IsTimeOut()
{
	if (m_eRoomState == ProtoMsg::ERoomStatus_None)
	{
		//超过秒未创建房间的解散
		if (m_iCurTime >= MAX_WAIT_CREATEROOM)
			return true;
	}
	else if (m_iCurTime >= (m_stMapInfo.m_iSecond + 180) * 1000)
		return true;

	return false;
}

int CRoom::GetSubSec()
{
	if (m_iCurTime >= m_stMapInfo.m_iSecond * 1000)
		return 0;
	else
		return (m_stMapInfo.m_iSecond - m_iCurTime / 1000);
}

int CRoom::GetSubWaitSec()
{
	if (m_eRoomState == ProtoMsg::ERoomStatus_None)
	{
		if (m_iCurTime >= MAX_WAIT_CREATEROOM)
			return 0;
		else
			return MAX_WAIT_CREATEROOM - m_iCurTime;
	}
	return 0;
}

bool CRoom::SendCreateRoom(CBattleSession* pBattle)
{
	if (pBattle == nullptr)
		return false;

	if (m_strCreateProto.size() > 0)
	{
		SetGroupId(pBattle->GetServerGroup());
		SetBattleSvrId(pBattle->GetServerId());
		pBattle->Send_Msg((const unsigned char*)m_strCreateProto.c_str(), m_strCreateProto.size()
			, MsgModule_ServerBattle::Msg_ServerBattle_CB_CreateRoom_Req, MsgModule::ServerInner);
		return true;
	}
	return false;
}

void CRoom::SendCreateRoomRes(ProtoMsg::ResultCode eCode)
{
	static Msg_ServerBattle_CB_CreateRoom_Rsp oCreateRsp;
	oCreateRsp.Clear();
	oCreateRsp.set_llroomid(m_llRoomId);
	oCreateRsp.set_strroomlogid(m_strLogId);
	oCreateRsp.set_stroldlogid(m_strLogId);
	oCreateRsp.set_ecode(eCode);
	oCreateRsp.set_strip("");
	oCreateRsp.set_iport(0);
	oCreateRsp.set_iroomareaid(0);
	
	if (gBCenterLogic->m_pCrossSession != nullptr)
		gBCenterLogic->m_pCrossSession->Send_Msg(&oCreateRsp, MsgModule_ServerBattle::Msg_ServerBattle_CB_CreateRoom_Rsp, ProtoMsg::ServerBattle);

	Log_Custom("create", "roomid:%lld, logid:%s", oCreateRsp.llroomid(), oCreateRsp.strroomlogid().c_str());
}

void CRoom::DoOnverProcess()
{
	if (IsTimeOut())
		Log_Warning("room timeout id:%lld", m_llRoomId);
}