#include "CBattleSession.h"
#include "CBattleManager.h"
#include "CRoomManager.h"
#include "log_mgr.h"
#include "parse_pb.h"
#include "CRoom.h"
#include "CCrossSession.h"
#include "msg_module_serverbattle.pb.h"
#include "CBCenterLogic.h"

using namespace ProtoMsg;

CBattleSession::CBattleSession()
{
	m_iRoomNum = 0;
}

CBattleSession::~CBattleSession()
{
}

void CBattleSession::Init()
{
	svr_session::Init();
	m_iBindPort = 0;
	m_iMaxRoomNum = 0;
	m_iRoomNum = 0;
	m_iUserNum = 0;
	m_iServerGroup = 0;
	m_iMaxUserNum = 0;
}

void CBattleSession::Init(ProtoMsg::Msg_ServerBattle_BC_Register_Req& oRegisterReq)
{
	//初始化数据
	InitData(oRegisterReq.strip(), oRegisterReq.iport());
	m_iServerGroup = oRegisterReq.iserverarea();
	m_iMaxUserNum = oRegisterReq.imaxusernum();
	m_iUserNum = oRegisterReq.icuruser();
	m_iRoomNum = oRegisterReq.icurroom();
	m_iMaxRoomNum = oRegisterReq.imaxroomnum();
	m_strLinkIp = oRegisterReq.strlinkip();
	
}

void CBattleSession::Release()
{
	svr_session::Release();
	m_strBindIp.clear();
}


void CBattleSession::handle_msg(const tagMsgHead* pMsg)
{}

void CBattleSession::handle_msg(msg_id_t usCmd, const unsigned char* pMsg, size_t uiSize)
{
	switch (usCmd)
	{
	case MsgModule_ServerBattle::Msg_ServerBattle_CB_CreateRoom_Rsp:
		OnCreateRoomRsp(pMsg, uiSize);
		break;
	case MsgModule_ServerBattle::Msg_ServerBattle_BC_RoomInfo_Notify:
		OnRoomInfoSubmit(pMsg, uiSize);
		break;
	case MsgModule_ServerBattle::Msg_ServerBattle_BC_LoadInfo_Notify:
		OnLoadInfoSubmit(this, pMsg, uiSize);
		break;
	case  MsgModule_ServerBattle::Msg_ServerBattle_BC_Result_Notify:
		OnBattleResultNotify(pMsg, uiSize);
		break;
	default:
		Log_Error("undefine cmd %u!", usCmd);
		break;
	}
}

void CBattleSession::on_disconnect()
{
	Log_Warning("battle server ip:%s,port:%d disconnect!", m_strBindIp.c_str(), m_iBindPort);
	gBattleManager->UnRegBattleSvr(this);
}

void CBattleSession::disconnected()
{
	CLogicSession::disconnected();
}

void CBattleSession::InitData(const std::string& strBindIp, int iBindPort) 
{
	m_strBindIp = strBindIp;
	m_iBindPort = iBindPort;
	m_iRoomNum = 0;

	gBattleManager->GetBattleDisTs(GetServerId());
}

int CBattleSession::GetLeftRoom()
{
	if (m_iRoomNum >= m_iMaxRoomNum)
		return 0;
	return m_iMaxRoomNum - m_iRoomNum;
}

int CBattleSession::GetLeftUser()
{
	if (m_iUserNum >= m_iMaxUserNum)
		return 0;
	return m_iMaxUserNum - m_iUserNum;
}

void CBattleSession::PushWaitLst()
{
	if (gRoomManager->WaitSize() == 0)
		return;

	//等待队列的直接加进去
	int iLeftRoom = GetLeftRoom();
	int iLeftUser = GetLeftUser();
	if (iLeftRoom == 0 || iLeftUser == 0)
		return;

	int iRoomRoom = 0;
	int iUserNum = 0;

	//检查排队队列
	while (gRoomManager->WaitSize() > 0)
	{
		if (iRoomRoom >= iLeftRoom)
			return;

		if (iUserNum >= iLeftUser)
			return;

		int64_t llId = gRoomManager->PopWaitId();
		CRoom* pRoom = gRoomManager->GetRoom(llId);

		if (pRoom != nullptr)
		{
			if (pRoom->GetSubWaitSec() > 3000)			//保留3s的创建时间
			{
				if (pRoom->SendCreateRoom(this))
				{
					iUserNum += pRoom->GetUserNum();
					++iRoomRoom;
				}
				else
					pRoom->SendCreateRoomRes(Code_Match_FullRoom);
			}
			else
				pRoom->SendCreateRoomRes(Code_Match_FullRoom);
		}
		else
			Log_Error("room error!!!, id:%lld", llId);
	}
}

void CBattleSession::OnCreateRoomRsp(const unsigned char* pMsg, size_t uiSize)
{
	Msg_ServerBattle_CB_CreateRoom_Rsp oRsp;
	PARSE_PTL(oRsp, pMsg, uiSize);

	CRoom* pRoom = gRoomManager->GetRoom(oRsp.llroomid());
	if (nullptr == pRoom || pRoom->GetRoomLogId() != oRsp.stroldlogid())
	{
		Log_Error("pRoom %lld is nullptr!", oRsp.llroomid());
	}
	else
	{
		pRoom->ClearCreateProto();
		//转发world
		if (gBCenterLogic->m_pCrossSession != nullptr)
			gBCenterLogic->m_pCrossSession->Send_Msg(pMsg, uiSize, MsgModule_ServerBattle::Msg_ServerBattle_CB_CreateRoom_Rsp, ServerBattle);

		if (ResultCode::Code_Common_Success == oRsp.ecode())
		{
			pRoom->SetIp(oRsp.strip());
			pRoom->SetLinkIp(oRsp.strlinkip());
			pRoom->SetPort(oRsp.iport());
			pRoom->SetRoomLogId(oRsp.strroomlogid());
			pRoom->SetRoomState(ERoomStatus_Start);
		}
		else
		{
			pRoom->ClearRoom();
			gRoomManager->DelRoom(oRsp.llroomid());
		}
	}
}

void CBattleSession::OnRoomInfoSubmit(const unsigned char* pMsg, size_t uiSize)
{
	Msg_ServerBattle_BC_RoomInfo_Notify oNotify;
	PARSE_PTL(oNotify, pMsg, uiSize);

	CRoom* pRoom = gRoomManager->GetRoom(oNotify.llroomid());
	if (nullptr == pRoom || pRoom->GetRoomLogId() != oNotify.strroomlogid())
	{
		Log_Error("pRoom %lld is nullptr!", oNotify.llroomid());
		return;
	}
	pRoom->SetRoomState(oNotify.estatus());
	pRoom->SetInGameNum(oNotify.iusernum());
}

void CBattleSession::OnLoadInfoSubmit(CLogicSession* pSession, const unsigned char* pMsg, size_t uiSize)
{
	Msg_ServerBattle_BC_LoadInfo_Notify oNotify;
	PARSE_PTL(oNotify, pMsg, uiSize);

	CBattleSession* pWorkSession = dynamic_cast<CBattleSession*>(pSession);
	if (nullptr == pWorkSession)
	{
		Log_Error("battle session is nullptr");
		return;
	}
	pWorkSession->SetRoomNum(oNotify.iroomnum());
	pWorkSession->SetUserNum(oNotify.iusernum());
}

void CBattleSession::OnBattleResultNotify(const unsigned char* pMsg, size_t uiSize)
{
	Msg_ServerBattle_BC_Result_Notify oNotify;
	PARSE_PTL(oNotify, pMsg, uiSize);

	CRoom* pRoom = gRoomManager->GetRoom(oNotify.llroomid());
	if (nullptr == pRoom || pRoom->GetRoomLogId() != oNotify.strroomlogid())
	{
		Log_Error("pRoom %lld is nullptr!", oNotify.llroomid());
	}
	else
	{
		pRoom->SetRoomState(oNotify.oroomdata().estatus());

		//转发world
		if (gBCenterLogic->m_pCrossSession != nullptr)
			gBCenterLogic->m_pCrossSession->Send_Msg(pMsg, uiSize, MsgModule_ServerBattle::Msg_ServerBattle_BC_Result_Notify, ProtoMsg::ServerBattle);
		pRoom->ClearRoom();
		gRoomManager->DelRoom(oNotify.llroomid());
	}
	//弹出等待队列
	PushWaitLst();
}
