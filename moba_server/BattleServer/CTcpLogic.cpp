#include "parse_pb.h"
#include "CTcpLogic.h"
#include "CBattleLogic.h"
#include "msg_module_battle.pb.h"
#include "CTcpSess.h"
#include "CBattleUser.h"
#include "global_define.h"
#include "CRoom.h"
#include "CRoomThread.h"

CTcpLogic::CTcpLogic()
{
}

CTcpLogic::~CTcpLogic()
{
	if (m_thread.joinable())
		m_thread.join();

	Fini();
}

bool CTcpLogic::Init(int iPort, CRoomThread* pRoomNet)
{
	net_setting stSetting;
	stSetting.m_nListenPort = (uint16_t)iPort;

	//战斗服,单个进程的人数少,所以单线程效率可能更高点,并且还要保证包的顺序
	stSetting.m_ioThread.Init(1, 400, MAX_SIZE_64K, MAX_SIZE_256K, MAX_SIZE_256K, MAX_TECV_TIMES, TIME_OUT_SEC);
	stSetting.InitMq(EMQ_NO);

	m_pLogic = CLogicThread::newm(stSetting);
	if (m_pLogic == nullptr)
	{
		Log_Error("logic thread new failed!");
		return false;
	}
	m_pLogic->reg_interface_listen(this);

	m_pHeartEvent = new CTimeEvent(m_pLogic->evthread()->Base(), true, 50, std::bind(&CTcpLogic::OnTimerTcp, this, std::placeholders::_1));
	if (!m_pHeartEvent->init())
	{
		delete m_pHeartEvent;
		m_pHeartEvent = nullptr;
		return false;
	}
	m_thread = std::thread(&CTcpLogic::Run, this);
	m_pRoomNet = pRoomNet;
	m_iPort = iPort;
	return true;
}

void CTcpLogic::Run()
{
	if (m_pLogic == nullptr)
		return;
	m_pLogic->run();
}

void CTcpLogic::handle_logic_msg(const tagNetMsg* pNetMsg)
{
	const tagHostHd& stHead = pNetMsg->m_hd;
	tagMsgHead* pMsgHead = NET_HEAD_MSG(pNetMsg->m_body);
	if (pMsgHead == nullptr)
		return;

	if (pMsgHead->usModuleId != MsgModule::Battle)
	{
		Log_Error("host module:%u, cmd:%u", pMsgHead->usModuleId, pMsgHead->uiCmdId);
		CLogicSession* pSession = get_session(pNetMsg->m_hd);
		if (pSession != nullptr)
		{
			Log_Error("kill session, module:%u, cmd:%u", pMsgHead->usModuleId, pMsgHead->uiCmdId);
			kill_session(pSession);
		}
		return;
	}

	//心跳包处理
	if (pMsgHead->uiCmdId == MsgModule_Battle::Msg_Battle_Beat_Req)
	{
		CLogicSession* pSession = get_session(pNetMsg->m_hd);
		if (pSession && pNetMsg->m_body)
		{
			CTcpSess* pTcp = dynamic_cast<CTcpSess*>(pSession);
			if (pTcp)
				pTcp->SetLastTime(GetCurrTime());
		}
		//心跳包直接发送回去
		Msg_Battle_Beat_Rsp oBeat;
		tagMsgHead* pHeadSend = MakeHeadMsg(&oBeat, pMsgHead->usModuleId,
			MsgModule_Battle::Msg_Battle_Beat_Rsp, pMsgHead->uiSeqid, Code_Common_Success);
		if (pHeadSend)
			my_send_conn_msg(stHead.m_threadOid, stHead.m_connOid, pHeadSend);
		return;
	}
	else if (pMsgHead->uiCmdId == MsgModule_Battle::Msg_Battle_EnterRoom_Req)
	{
		EnterRoomTcp(pMsgHead, stHead);
	}
	else
	{
		CLogicSession* pSession = get_session(pNetMsg->m_hd);
		if (pSession && pNetMsg->m_body)
			pSession->handle_msgv((const void*)(pNetMsg->m_body));
		else
			Log_Error("error session, module:%u, cmd:%u", pMsgHead->usModuleId, pMsgHead->uiCmdId);
	}
}

void CTcpLogic::KillUser(CBattleUser* pBtlUser)
{
	if (!m_pLogic)
	{
		pBtlUser->on_disconnect();
		return;
	}
	kill_session(pBtlUser->GetTcpSess());
}

void CTcpLogic::EnterUser(CBattleUser* pBtlUser)
{
}

void CTcpLogic::DeleteUser(CBattleUser* pBtlUser)
{
}

void CTcpLogic::AddRoom(CRoom* pRoom)
{
}

void CTcpLogic::RemoveRoom(CRoom* pRoom)
{
}

void  CTcpLogic::Stop()
{
	if (m_pHeartEvent)
		m_pHeartEvent->stop();
	if (m_pLogic != nullptr)
		m_pLogic->stop();
}

void CTcpLogic::Fini()
{
	SAFE_DELETE(m_pHeartEvent);
	CLogicThread::del(m_pLogic);
}

void CTcpLogic::OnTimerTcp(int iTime)
{
	if (m_pRoomNet != nullptr)
		m_pRoomNet->OnTimer(iTime);
}

void CTcpLogic::Tick(int64_t llTick)
{
}

bool CTcpLogic::my_send_conn_msg(thread_oid_t usToid, conn_oid_t usCoid, const tagMsgHead* pMsg)
{
	return ::my_send_conn_msg(m_pDispatcher, usToid, usCoid, pMsg);
}

void CTcpLogic::EnterRoomTcp(const tagMsgHead* pNetHead, const tagHostHd& stHead)
{
	if (m_pRoomNet == nullptr) return;

	Msg_Battle_EnterRoom_Req oEnterReq;
	PARSE_PTL_HEAD(oEnterReq, pNetHead);

	Msg_Battle_EnterRoom_Rsp oEnterRsp;
	oEnterRsp.set_strmapname("");
	oEnterRsp.set_etype(ERoomType_None);
	oEnterRsp.set_irandseed(0);

	tagMsgHead* pHeadSend = MakeHeadMsg(&oEnterRsp, pNetHead->usModuleId, 
		MsgModule_Battle::Msg_Battle_EnterRoom_Rsp, pNetHead->uiSeqid, Code_Match_RoomRelease);

	int64_t llRoomId = m_pRoomNet->GetRoomId(oEnterReq.strroomsessionid());
	CRoom* pRoom = m_pRoomNet->GetRoom(llRoomId);
	
	//检测房间是否存在
	if (pRoom == nullptr || pRoom->IsEnd() || pRoom->IsAllOver())
	{
		if (pHeadSend)
			my_send_conn_msg(stHead.m_threadOid, stHead.m_connOid, pHeadSend);

		Log_Warning("battle room error! roomid:%lld, roomsession:%s, usersession:%s",
			llRoomId, oEnterReq.strroomsessionid().c_str(), oEnterReq.strusersessionid().c_str());
		return;
	}
	//检测房间玩家数据是否存在
	user_battle_info_t* pUserData = pRoom->GetUserData(oEnterReq.strusersessionid(), true);
	if (pUserData == nullptr)
	{
		Log_Warning("user session error! rooid:%lld, usersession:%s", pRoom->GetRoomId(), oEnterReq.strusersessionid().c_str());
		if (pHeadSend)
			my_send_conn_msg(stHead.m_threadOid, stHead.m_connOid, pHeadSend);
		return;
	}
	//
	bool bNeedCreate = true;
	CBattleUser* pBtlUser = m_pRoomNet->GetUser(pUserData->ouserinfo().lluid());
	if (pBtlUser != nullptr)
	{
		if (pBtlUser->GetTcpSess()->IsSameConn(stHead))
		{
			Log_Warning("user enter again! roomid:%lld, usersession:%s", pRoom->GetRoomId(), oEnterReq.strusersessionid().c_str());
			pRoom->Enter(pBtlUser);
			bNeedCreate = false;
		}
		else
			kill_session(pBtlUser->GetTcpSess());
	}

	if (bNeedCreate)
	{
		pBtlUser = m_pRoomNet->CreateUser(pUserData->ouserinfo().lluid());
		if (pBtlUser == nullptr)
		{
			Log_Warning("user create error! roomid:%lld, usersession:%s", pRoom->GetRoomId(), oEnterReq.strusersessionid().c_str());
			if (pHeadSend)
				my_send_conn_msg(stHead.m_threadOid, stHead.m_connOid, pHeadSend);
			return;
		}
		pBtlUser->SetRoom(pRoom);
		pBtlUser->SetUserBaseData(*pUserData);
		pBtlUser->InitTcp();
		pRoom->Enter(pBtlUser);
		add_session(pBtlUser->GetTcpSess(), stHead, 0);
		pBtlUser->InitIp();
	}
	pBtlUser->OnEnterRoom(oEnterReq, pNetHead->uiSeqid);
}
