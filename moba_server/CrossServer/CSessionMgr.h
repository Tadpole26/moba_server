#pragma once

#include "CGameSession.h"
#include "server_pool.h"
#include "msg_module.pb.h"
#include "singleton.h"
using namespace ProtoMsg;

class CSessionMgr
{
public:
	CSessionMgr();
	~CSessionMgr();

	bool Init();
	void UnInit();
	void OnTimer();

	CGameSession* AddGameSvr(uint32_t uiServerId, uint32_t uiGroupId);
	void DelGameSvr(CGameSession* pServer);
	CGameSession* GetGameById(uint32_t uiId);

	void SendToGameServer(uint32_t uiServerId, const tagMsgHead* pMsg);
	bool SendToGameServer(uint32_t uiServerId, google::protobuf::Message* pMsg, msg_id_t cCmd
		, msg_id_t cModule = MsgModule::ServerCommon);

	void BroadAllGameServer(tagMsgHead* pMsg);
	bool BroadAllGameServer(google::protobuf::Message* pMsg, msg_id_t cCmd
		, msg_id_t cModule = MsgModule::ServerCommon);

	void SendToGameUser(uint32_t uiServerId, std::vector<int64_t>& vecUids
		, google::protobuf::Message* pMsg, msg_id_t cModule, msg_id_t cCmd
		, ResultCode eCode);
public:
	//mmo可能一个groupId对应多个serverId,卡牌groupid和serverid相同
	ServerPool<CGameSession> m_oGameMgr;

	std::set<uint32_t>	m_setGroups;
};

#define GAME_SESSIONS_INS Singleton<CSessionMgr>::getInstance()
