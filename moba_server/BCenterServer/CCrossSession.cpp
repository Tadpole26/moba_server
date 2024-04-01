#include "CCrossSession.h"
#include "CRoomManager.h"
#include "CBattleManager.h"
#include "log_mgr.h"
#include "parse_pb.h"
#include "msg_module_serverbattle.pb.h"
using namespace ProtoMsg;

CCrossSession::CCrossSession()
{
	m_eServerKind = SERVER_KIND_NONE;
	m_uiServerId = 0;
}

CCrossSession::~CCrossSession()
{
}

void CCrossSession::handle_msg(const tagMsgHead* pNetMsg)
{
	HandleMessage(pNetMsg->usModuleId, pNetMsg->uiCmdId, NET_DATA_BUF(pNetMsg), NET_DATA_SIZE(pNetMsg));
}

void CCrossSession::on_disconnect()
{
	Log_Warning("cross server %u disconnect!", m_uiServerId);
}

void CCrossSession::HandleMessage(msg_id_t usModule, uint32 uiCmd, const unsigned char* pData, size_t uiLen)
{
	if (usModule == ProtoMsg::ServerBattle)
	{
		switch (uiCmd)
		{
		case MsgModule_ServerBattle::Msg_ServerBattle_CB_CreateRoom_Req:
			OnCreateRoomReq(pData, uiLen);
			break;
		default:
			break;
		}
	}
	else if (usModule == ProtoMsg::ServerCommon)
	{
		HandleCommonModule(uiCmd, (unsigned char*)pData, uiLen);
	}
	else
	{
		Log_Error("undefined module %d!", usModule);
	}
}

void CCrossSession::HandleCommonModule(uint32 uiCmd, unsigned char* pMsg, size_t uiSize)
{}

void CCrossSession::OnCreateRoomReq(const unsigned char* pData, size_t uiLen)
{
	static Msg_ServerBattle_CB_CreateRoom_Req oCreateReq;
	PARSE_PTL(oCreateReq, pData, uiLen);

	CRoom* pRoom = gRoomManager->GetRoom(oCreateReq.llroomid());
	if (pRoom != nullptr)
	{
		//存在旧的,表示断网异常处理
		gRoomManager->DelRoom(oCreateReq.llroomid());
	}

	pRoom = gRoomManager->CreateRoom(oCreateReq.llroomid());
	if (pRoom != nullptr)
	{
		if (pRoom->Init(oCreateReq))
		{
			pRoom->SetCrossSessionId(GetServerId());
			pRoom->SetCrossType(GetServerKind());
			pRoom->SetCreateProto(pData, uiLen);
			CBattleSession* pBattle = gBattleManager->FindServer();
			if (pBattle != nullptr)
			{
				pRoom->SendCreateRoom(pBattle);
				return;
			}
			else if (gBattleManager->GetBattleSize() != 0)
			{
				if (oCreateReq.bwaitlst())			//开始排队
				{
					gRoomManager->AddWaitId(pRoom->GetRoomId());
					return;
				}
			}
		}
		gRoomManager->DelRoom(oCreateReq.llroomid());
	}

	static Msg_ServerBattle_CB_CreateRoom_Rsp oCreateRsp;
	oCreateRsp.Clear();
	oCreateRsp.set_llroomid(oCreateReq.llroomid());
	oCreateRsp.set_strroomlogid(oCreateReq.strroomlogid());
	oCreateRsp.set_stroldlogid(oCreateReq.strroomlogid());
	oCreateRsp.set_ecode(ResultCode::Code_Match_FullRoom);
	oCreateRsp.set_strip("");
	oCreateRsp.set_iport(0);
	oCreateRsp.set_iroomareaid(0);

	Send_Msg(&oCreateRsp, MsgModule_ServerBattle::Msg_ServerBattle_CB_CreateRoom_Rsp, ProtoMsg::ServerBattle);

	Log_Custom("create", "roomid:%lld, logid:%s", oCreateReq.llroomid(), oCreateReq.strroomlogid().c_str());
}