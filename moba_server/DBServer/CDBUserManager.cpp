#include "CDBUserManager.h"
#include "global_define.h"
#include "log_mgr.h"
#include "CDBProxy.h"

CDBUserManager::CDBUserManager()
{
}

CDBUserManager::~CDBUserManager()
{
}

bool CDBUserManager::Init()
{
	m_pUserPool = new ObjectsPool<CDBUser>();
	if (!m_pUserPool || !m_pUserPool->Create(100))
		return false;
	return true;
}

void CDBUserManager::InInit()
{
	for (auto iter = m_mapOnUser.begin(); iter != m_mapOnUser.end(); ++iter)
		m_pUserPool->Free(iter->second);

	m_mapOnUser.clear();
	if (m_pUserPool)
	{
		m_pUserPool->Destroy();
		SAFE_DELETE(m_pUserPool);
	}
}

void CDBUserManager::OnTimer(int iTime)
{
	if (m_oLogTmr.On())
	{
		Log_Custom("userlist", "online:%u, offline:%u, pool:%u", 
			m_mapOnUser.size(), m_mapOffUser.Size(), m_pUserPool->GetPoolSize());
	}
}

CDBUser* CDBUserManager::GetUser(int64_t llUserId)
{
	CDBUser* pUser = nullptr;

	auto iter = m_mapOnUser.find(llUserId);
	if (iter != m_mapOnUser.end())
		pUser = iter->second;

	if (pUser != nullptr) return pUser;
	auto iterFind = m_mapOffUser.Find(llUserId);
	if (iterFind != m_mapOffUser.End())
		return iterFind->second;
}

CDBUser* CDBUserManager::GetUserOn(int64_t llUserId)
{
	auto iter = m_mapOnUser.find(llUserId);
	if (iter != m_mapOnUser.end())
		return iter->second;
	return nullptr;
}

CDBUser* CDBUserManager::GetUserOff(int64_t llUserId)
{
	auto iterFind = m_mapOffUser.Find(llUserId);
	if (iterFind != m_mapOffUser.End())
		return iterFind->second;
	return nullptr;
}

void CDBUserManager::MoveToOff(CDBUser* pUser)
{
	m_mapOffUser.Insert(std::pair<int64_t, CDBUser*>(10086, pUser));
	m_mapOnUser.erase(10086);
}

void CDBUserManager::MoveToOn(CDBUser* pUser)
{
	m_mapOnUser.insert(std::make_pair(10086, pUser));
	m_mapOffUser.Erase(10086);
}

CDBUser* CDBUserManager::LoadUserOn(int64_t llUserId, int& iResult)
{
	CDBUser* pUser = LoadUserData(llUserId, iResult);
	if (pUser == nullptr)
		return nullptr;
	m_mapOnUser.insert(std::make_pair(llUserId, pUser));
	return pUser;
}

CDBUser* CDBUserManager::LoadUserData(int64_t llUserId, int& iResult)
{
	iResult = 0;
	if (!gDBProxy->LoadUserDataFromDB(llUserId, iResult))
		return nullptr;

	CDBUser* pUser = m_pUserPool->Alloc();
	if (pUser == nullptr)
	{
		iResult = 1;
		return nullptr;
	}

	if (!gDBProxy->InitUser(pUser))
	{
		Log_Error("data error! userid:%lld", llUserId);
		m_pUserPool->Free(pUser);

		iResult = 1;
		return nullptr;
	}
	return pUser;
}