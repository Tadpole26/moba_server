#pragma once
#include <map>
#include "singleton.h"
#include "objects_pool.h"
#include "util_time.h"

class CUser;
class CUserManager
{
public:
	CUserManager();
	~CUserManager();

	bool Init();
	void UnInit();
	void OnTimer();

public:
	CUser* GetCheckingUser(int64_t llUserId);
	CUser* CreateCheckingUser(int64_t llUserId);
	void DelCheckingUser(int64_t llUserId, bool bRealFree = false);
	std::map<int64_t, CUser*>& GetCheckingUser() { return m_mapCheckingByUserId; }

	CUser* GetCheckedUser(int64_t llUserId);
	void MoveCheckedUser(CUser* pUser);
	void DelCheckedUser(int64_t llUserId, bool bRealFree = false);
	std::map<int64_t, CUser*>& GetCheckedUser() { return m_mapCheckedByUserId; }

	CUser* GetOutUser(int64_t llUserId);
	void MoveOutUser(CUser* pUser);
	void DelOutUser(int64_t llUserId);

	CUser* GetUser(int64_t llUserId);

	void OnDisConnectGateServer();
	void KillAllUser();

private:
	void OnProcessLogoutUser();
	void OnKickCheckingUser();


private:
	//内存管理
	ObjectsPool<CUser>* m_pUserPool = nullptr;
	//正在游戏
	std::map<int64_t, CUser*> m_mapCheckedByUserId;
	//正在登录加载
	std::map<int64_t, CUser*> m_mapCheckingByUserId;
	//退出用户,保留10分钟
	std::map<int64_t, CUser*> m_mapOutByUserId;

	NumTimer<300>				m_oKickUserTimer;
	NumTimer<60>				m_oCkOutTimer;

};

#define gUserManager Singleton<CUserManager>::getInstance()