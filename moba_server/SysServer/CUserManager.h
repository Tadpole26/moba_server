#pragma once
#include "map_pool.h"
#include "CUser.h"
#include "singleton.h"

class CUserManager
{
public:
	CUser* GetUser(int64 llUid);

	CUser* AddUser(int64 llUid);

	void DelUser(int64 llUid);

	void DelUserByGateId(uint32 uiGateId);
private:
	MapPool<int64, CUser> m_mapUser;
};

#define SYS_PLAYER_MGR_INS Singleton<CUserManager>::getInstance()
