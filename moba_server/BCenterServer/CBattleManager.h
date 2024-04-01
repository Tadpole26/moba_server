#pragma once

#include <map>
#include <list>
#include "CBattleSession.h"
#include "server_pool.h"

class CBCenterLogic;
class CBattleSession;
class CBattleManager
{
public:
	CBattleManager();
	~CBattleManager();

public:
	bool Init();
	void UnInit();
	void OnTimer();

	CBattleSession* AddBattleSvr(int iServerId, int iMaxRoom);
	void SetBattleGroupIp(int iGroupId, std::string strIp);

	void UnRegBattleSvr(CBattleSession* pServer);
	int GetBattleDisTs(int iServerId);

	CBattleSession* GetBattleServerById(int iId);
	CBattleSession* FindServer();
	size_t GetBattleSize();

	int GetMaxRoom() { return m_iMaxRoom; }

	int GetBattleSvrNum() { return (int)m_vecBattle.size(); }

private:
	CBattleSession* FindServerBest();

	ServerPool<CBattleSession> m_oBattleMgr;
	std::vector<CBattleSession*> m_vecBattle;
	std::map<int, int> m_mapBattleDiscon;
	std::map<int, std::string> m_mapBattleIp;

	int m_iMaxRoom = 0;
};

#define gBattleManager Singleton<CBattleManager>::getInstance()