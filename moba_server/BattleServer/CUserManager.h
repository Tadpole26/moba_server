#pragma once
#include <cstdint>
#include "map_pool.h"
#include "CBattleUser.h"
#include "util_time.h"
#include "singleton.h"

class CUserManager
{
public:
	CUserManager();
	~CUserManager();

	void  AddUser(int64_t llUserId,int iConvId);

	void DelUser(const std::map<int64_t, int>& mapId);
	void DelUser(int64_t llUserId,int iConvId);
	size_t GetUserNum();
	void OnTimer(int32_t iTime);
private:
	std::mutex m_oLock;
	std::map<int64_t, int> m_mapUserId;
	NumTimer<60000> m_tmrLog;
};

#define gUserManager Singleton<CUserManager>::getInstance()