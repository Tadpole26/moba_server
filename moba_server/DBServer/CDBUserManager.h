#pragma once

#include "singleton.h"
#include "objects_pool.h"
#include "CDBUser.h"
#include "lru_hashmap.h"

class CDBUserManager
{
public:
	CDBUserManager();
	~CDBUserManager();

	bool Init();
	void InInit();
	void OnTimer(int iTime);

	//获取玩家数据
	CDBUser* GetUser(int64_t llUserId);
	//获取在线玩家数据
	CDBUser* GetUserOn(int64_t llUserId);
	//获取离线玩家数据
	CDBUser* GetUserOff(int64_t llUserId);

	//由在线移到离线列表
	void MoveToOff(CDBUser* pUser);
	//有离线移到在线列表
	void MoveToOn(CDBUser* pUser);

	CDBUser* LoadUserOn(int64_t llUserId, int& iResult);

	std::map<int64_t, CDBUser*>& GetAllUser() { return m_mapOnUser; }

protected:
	CDBUser* LoadUserData(int64_t llUserId, int& iResult);

private:
	ObjectsPool<CDBUser>* m_pUserPool = nullptr;
	//在线玩家列表
	std::map<int64_t, CDBUser*> m_mapOnUser;
	//离线玩家列表
	LogicLRUHashmap<int64_t, CDBUser*> m_mapOffUser;

	NumTimer<300> m_oLogTmr;
};

#define gDBUserManager Singleton<CDBUserManager>::getInstance()
