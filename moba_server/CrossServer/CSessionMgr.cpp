#include "CSessionMgr.h"
#include "log_mgr.h"
#include "msg_module_servercommon.pb.h"

CSessionMgr::CSessionMgr()
{
}

CSessionMgr::~CSessionMgr()
{
}

bool CSessionMgr::Init()
{
	return true;
}

void CSessionMgr::UnInit()
{
}

void CSessionMgr::OnTimer()
{
}

CGameSession* CSessionMgr::AddGameSvr(uint32_t uiServerId, uint32_t uiGroupId)
{
	CGameSession* pSession = m_oGameMgr.AddServer(SERVER_KIND_GAME, uiServerId);
	if (!pSession)
	{
		Log_Error("pSession is nullptr!!!");
		return nullptr;
	}
	m_setGroups.insert(uiGroupId);
	return pSession;
}

void CSessionMgr::DelGameSvr(CGameSession* pServer)
{
	m_setGroups.erase(pServer->m_uiGroupId);
	m_oGameMgr.DelServer(SERVER_KIND_GAME, pServer->GetServerId());
}

CGameSession* CSessionMgr::GetGameById(uint32_t uiId)
{
	return m_oGameMgr.GetServer(SERVER_KIND_GAME, uiId);
}

void CSessionMgr::SendToGameServer(uint32_t uiServerId, const tagMsgHead* pMsg)
{
	CGameSession* pSession = m_oGameMgr.GetServer(SERVER_KIND_GAME, uiServerId);
	if (!pSession)
	{
		Log_Error("pSession is nullptr!!!");
		return;
	}
	pSession->Send(pMsg);
}

bool CSessionMgr::SendToGameServer(uint32_t uiServerId, google::protobuf::Message* pMsg, msg_id_t cCmd
	, msg_id_t cModule)
{
	if (uiServerId != 0)
	{
		CGameSession* pSession = m_oGameMgr.GetServer(SERVER_KIND_GAME, uiServerId);
		if (!pSession)
		{
			Log_Error("pSession is nullptr!!!");
			return false;
		}
		return pSession->Send_Msg(pMsg, cCmd, cModule);
	}
	else
	{
		std::map<uint32_t, CGameSession*>& mapSvr = m_oGameMgr.GetServerMap(SERVER_KIND_GAME);
		for (auto iter = mapSvr.begin(); iter != mapSvr.end(); ++iter)
		{
			if (iter->second != nullptr)
				iter->second->Send_Msg(pMsg, cCmd, cModule);
		}
		return true;
	}
}

void CSessionMgr::BroadAllGameServer(tagMsgHead* pMsg)
{
	std::map<uint32_t, CGameSession*>& mapSvr = m_oGameMgr.GetServerMap(SERVER_KIND_GAME);
	for (auto iter = mapSvr.begin(); iter != mapSvr.end(); ++iter)
	{
		if (iter->second != nullptr)
			iter->second->Send(pMsg);
	}
}
bool CSessionMgr::BroadAllGameServer(google::protobuf::Message* pMsg, msg_id_t cCmd
	, msg_id_t cModule)
{
	std::map<uint32_t, CGameSession*>& mapSvr = m_oGameMgr.GetServerMap(SERVER_KIND_GAME);
	for (auto iter = mapSvr.begin(); iter != mapSvr.end(); ++iter)
	{
		if (iter->second != nullptr)
			iter->second->Send_Msg(pMsg, cCmd, cModule);
	}
	return true;
}

void CSessionMgr::SendToGameUser(uint32_t uiServerId, std::vector<int64_t>& vecUids
	, google::protobuf::Message* pMsg, msg_id_t cModule, msg_id_t cCmd
	, ResultCode eCode)
{
	Msg_ServerCommon_Transmit oTrans;
	for (auto& uid : vecUids)
	{
		oTrans.add_vecuids(uid);
	}
	oTrans.set_uimoduleid(cModule);
	oTrans.set_uicmd(cCmd);
	oTrans.set_uiretcode(eCode);
	oTrans.set_strcmdmsg(pMsg->SerializeAsString());
	SendToGameServer(uiServerId, &oTrans, MsgModule_ServerCommon::Msg_ServerCommon_Transmit, 
		cModule);
}