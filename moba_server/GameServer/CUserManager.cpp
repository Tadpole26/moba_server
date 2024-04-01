#include "CUserManager.h"
#include "util_time.h"
#include "log_mgr.h"
#include "global_define.h"
#include "CUser.h"

CUserManager::CUserManager()
{
	m_mapCheckedByUserId.clear();
	m_mapCheckingByUserId.clear();
	m_mapOutByUserId.clear();
}

CUserManager::~CUserManager()
{
}

bool CUserManager::Init()
{
	m_pUserPool = new ObjectsPool<CUser>;
	if (!m_pUserPool)
		return false;

	//time_t llTime = GetCurrTime();

	return (bool)m_pUserPool->Create(100);
}


void CUserManager::UnInit()
{
	for (auto& iter : m_mapCheckedByUserId)
		m_pUserPool->Free(iter.second);

	for (auto& iter : m_mapCheckingByUserId)
		m_pUserPool->Free(iter.second);

	for (auto& iter : m_mapOutByUserId)
		m_pUserPool->Free(iter.second);

	m_mapCheckedByUserId.clear();
	m_mapCheckingByUserId.clear();
	m_mapOutByUserId.clear();

	if (m_pUserPool)
	{
		m_pUserPool->Destroy();
		SAFE_DELETE(m_pUserPool);
	}
}

void CUserManager::OnTimer()
{
	//1分钟检查一次
	if (m_oCkOutTimer.On())
	{
		OnProcessLogoutUser();
		Log_Custom("userlist", "ingame:%u, checking:%u, logout:%u cache:%d",
			m_mapCheckedByUserId.size(), m_mapCheckingByUserId.size(), m_mapOutByUserId.size(), 
			m_pUserPool->GetPoolSize());
	}

	if (m_oKickUserTimer.On())
		OnKickCheckingUser();
}

CUser* CUserManager::GetCheckingUser(int64_t llUserId)
{
	auto iter = m_mapCheckingByUserId.find(llUserId);
	if (iter == m_mapCheckingByUserId.end())
		return iter->second;
	return nullptr;
}

CUser* CUserManager::CreateCheckingUser(int64_t llUserId)
{
	auto iter = m_mapCheckingByUserId.find(llUserId);
	if (iter == m_mapCheckingByUserId.end())
		return iter->second;

	CUser* pUser = m_pUserPool->Alloc();
	if (!pUser)
		return nullptr;
	m_mapCheckingByUserId.insert(std::make_pair(llUserId, pUser));
	return pUser;
}

void CUserManager::DelCheckingUser(int64_t llUserId, bool bRealFree)
{
	auto iter = m_mapCheckingByUserId.find(llUserId);
	if (iter != m_mapCheckingByUserId.end())
	{
		CUser* pUser = iter->second;
		m_mapCheckingByUserId.erase(iter);

		if (bRealFree && pUser)
		{
			m_pUserPool->Free(pUser);
			pUser = nullptr;
		}
	}
}

CUser* CUserManager::GetCheckedUser(int64_t llUserId)
{
	auto iter = m_mapCheckedByUserId.find(llUserId);
	if (iter == m_mapCheckedByUserId.end())
		return iter->second;
	return nullptr;
}

void CUserManager::MoveCheckedUser(CUser* pUser)
{
	assert(pUser);
	int64_t llUserId = pUser->GetUserId();
	auto iterOut = m_mapOutByUserId.find(llUserId);
	if (iterOut != m_mapOutByUserId.end())
	{
		if (iterOut->second != pUser)
		{
			Log_Error("user=%lld", llUserId);
			m_pUserPool->Free(iterOut->second);
		}
		m_mapOutByUserId.erase(iterOut);
	}

	auto iterChecking = m_mapCheckingByUserId.find(llUserId);
	if (iterChecking != m_mapCheckingByUserId.end())
	{
		if (iterChecking->second != pUser)
		{
			Log_Error("user=%lld", llUserId);
			m_pUserPool->Free(iterChecking->second);
		}
		m_mapCheckingByUserId.erase(iterChecking);
	}

	auto iterChecked = m_mapCheckedByUserId.find(llUserId);
	if (iterChecked != m_mapCheckedByUserId.end())
	{
		if (iterChecked->second != pUser)
		{
			Log_Error("user=%lld", llUserId);
			m_pUserPool->Free(iterChecked->second);
		}
	}
	m_mapCheckedByUserId.insert(std::make_pair(llUserId, pUser));
}

void CUserManager::DelCheckedUser(int64_t llUserId, bool bRealFree)
{
	auto iter = m_mapCheckedByUserId.find(llUserId);
	if (iter != m_mapCheckedByUserId.end())
	{
		CUser* pUser = iter->second;
		if (!pUser)
		{
			m_mapCheckedByUserId.erase(iter);
			return;
		}
		m_mapCheckedByUserId.erase(iter);
		if (bRealFree)
		{
			Log_Info("userid:%lld delete from cache", pUser->GetUserId());
			m_pUserPool->Free(pUser);
			pUser = nullptr;
		}
		else
		{
			MoveOutUser(pUser);
		}
	}
}

CUser* CUserManager::GetOutUser(int64_t llUserId)
{
	auto iter = m_mapOutByUserId.find(llUserId);
	if (iter == m_mapOutByUserId.end())
		return iter->second;
	return nullptr;
}

void CUserManager::MoveOutUser(CUser* pUser)
{
	pUser->SetLeaveTime(GetCurrTime());
	auto iter = m_mapOutByUserId.find(pUser->GetUserId());
	if (iter != m_mapOutByUserId.end())
	{
		if (iter->second != pUser)
		{
			Log_Error("error userid:%lld", pUser->GetUserId());
			m_pUserPool->Free(iter->second);
			m_mapOutByUserId.erase(iter);
		}
	}
	m_mapOutByUserId.insert(std::make_pair(pUser->GetUserId(), pUser));
}

void CUserManager::DelOutUser(int64_t llUserId)
{
	auto iter = m_mapOutByUserId.find(llUserId);
	if (iter != m_mapOutByUserId.end())
	{
		CUser* pUser = iter->second;
		m_mapOutByUserId.erase(iter);
		if (pUser)
		{
			Log_Info("userid:%lld delete from cache", pUser->GetUserId());
			m_pUserPool->Free(pUser);
			pUser = nullptr;
		}
	}
}

CUser* CUserManager::GetUser(int64_t llUserId)
{
	CUser* pUser = GetCheckedUser(llUserId);
	if (pUser) return pUser;
	return GetOutUser(llUserId);
}

void CUserManager::OnProcessLogoutUser()
{
	time_t llCurTime = GetCurrTime();
	auto iter = m_mapOutByUserId.begin();
	while (iter != m_mapOutByUserId.end())
	{
		CUser* pUser = iter->second;
		if (!pUser)
		{
			m_mapOutByUserId.erase(iter++);
			continue;
		}
		++iter;

		if ((pUser->GetLeaveTime() > 0) &&
			(llCurTime - pUser->GetLeaveTime() > 600))
		{
			DelOutUser(pUser->GetUserId());
		}
	}
}

void CUserManager::OnKickCheckingUser()
{
	time_t llCurTime = GetCurrTime();
	CUser* pUser = nullptr;
	//超过十分钟没有加载成功,或者没有创建角色的直接删掉下线
	int iTimeOut = 600;
	for (auto iter = m_mapCheckingByUserId.begin(); iter != m_mapCheckingByUserId.end();)
	{
		pUser = iter->second;
		if (pUser->GetLeaveTime() + iTimeOut < llCurTime)
		{
			//剔除旧的连接
			pUser->SendKickUserNotify(Code_Common_Failure);
			iter = m_mapCheckingByUserId.erase(iter);
			m_pUserPool->Free(pUser);
		}
		else
			++iter;
	}
}

void CUserManager::OnDisConnectGateServer()
{
	time_t llCurTime = GetCurrTime();
	auto iter = m_mapCheckedByUserId.begin();
	while (iter != m_mapCheckedByUserId.end())
	{
		CUser* pUser = iter->second;
		if (!pUser)
		{
			m_mapCheckedByUserId.erase(iter++);
			continue;
		}
		++iter;

		pUser->LeaveGame(false);
		DelCheckedUser(pUser->GetUserId(), false);
	}
	Log_Info("all user eleave success!");
}

void CUserManager::KillAllUser()
{
	CUser* pUser = nullptr;

	for (auto iter = m_mapCheckingByUserId.begin(); iter != m_mapCheckingByUserId.end();)
	{
		pUser = iter->second;
		++iter;

		m_pUserPool->Free(pUser);
	}

	for (auto iter = m_mapCheckedByUserId.begin(); iter != m_mapCheckedByUserId.end();)
	{
		pUser = iter->second;
		++iter;

		pUser->LeaveGame(false);
		m_pUserPool->Free(pUser);
	}

	for (auto iter = m_mapOutByUserId.begin(); iter != m_mapOutByUserId.end();)
	{
		pUser = iter->second;
		++iter;

		m_pUserPool->Free(pUser);
	}

	m_mapCheckingByUserId.clear();
	m_mapCheckedByUserId.clear();
	m_mapOutByUserId.clear();
}