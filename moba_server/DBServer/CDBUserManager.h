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

	//��ȡ�������
	CDBUser* GetUser(int64_t llUserId);
	//��ȡ�����������
	CDBUser* GetUserOn(int64_t llUserId);
	//��ȡ�����������
	CDBUser* GetUserOff(int64_t llUserId);

	//�������Ƶ������б�
	void MoveToOff(CDBUser* pUser);
	//�������Ƶ������б�
	void MoveToOn(CDBUser* pUser);

	CDBUser* LoadUserOn(int64_t llUserId, int& iResult);

	std::map<int64_t, CDBUser*>& GetAllUser() { return m_mapOnUser; }

protected:
	CDBUser* LoadUserData(int64_t llUserId, int& iResult);

private:
	ObjectsPool<CDBUser>* m_pUserPool = nullptr;
	//��������б�
	std::map<int64_t, CDBUser*> m_mapOnUser;
	//��������б�
	LogicLRUHashmap<int64_t, CDBUser*> m_mapOffUser;

	NumTimer<300> m_oLogTmr;
};

#define gDBUserManager Singleton<CDBUserManager>::getInstance()
