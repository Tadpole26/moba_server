#include "CUserManager.h"
#include "log_mgr.h"

CUserManager::CUserManager()
{
}

CUserManager::~CUserManager()
{
	m_mapUserId.clear();
}

void CUserManager::AddUser(int64_t llUserId, int iConvId)
{
	if (llUserId == 0) return;

	m_oLock.lock();
	m_mapUserId[llUserId] = iConvId;
	m_oLock.unlock();
}

void CUserManager::DelUser(int64_t llUserId, int iConvId)
{
	m_oLock.lock();
	auto iter = m_mapUserId.find(llUserId);
	if (iter != m_mapUserId.end() && iter->second == iConvId)
		m_mapUserId.erase(iter);
	m_oLock.unlock();
}

void CUserManager::DelUser(const std::map<int64_t, int>& mapId)
{
	m_oLock.lock();
	for (auto& iterId : mapId)
	{
		auto iter = m_mapUserId.find(iterId.first);
		if (iter != m_mapUserId.end() && iter->second == iterId.second)
			m_mapUserId.erase(iter);
	}
	m_oLock.unlock();
}

size_t CUserManager::GetUserNum()
{
	m_oLock.lock();
	size_t uiNum = m_mapUserId.size();
	m_oLock.unlock();
	return uiNum;
}

void CUserManager::OnTimer(int32_t iTime)
{
	if (m_tmrLog.On(iTime))
		Log_Custom("userlist", "size:%u", GetUserNum());
}

