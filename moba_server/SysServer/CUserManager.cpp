#include "CUserManager.h"

CUser* CUserManager::GetUser(int64 llUid)
{
	if (llUid == 0) return nullptr;
	return m_mapUser.find(llUid);
}

CUser* CUserManager::AddUser(int64 llUid)
{
	if (llUid == 0) return nullptr;

	CUser* pPlayer = m_mapUser.find(llUid);
	if (pPlayer != nullptr)
		return pPlayer;

	pPlayer = m_mapUser.insert(llUid);
	if (pPlayer == nullptr)
		return nullptr;
	pPlayer->m_llUid = llUid;
	return pPlayer;
}

void CUserManager::DelUser(int64 llUid)
{
	m_mapUser.erase(llUid);
}

void CUserManager::DelUserByGateId(uint32 uiGateId)
{
	const std::map<int64, CUser*>& mapPlayer = m_mapUser.tickmap();
	std::vector<int64> vDel;

	for (const auto& iter : mapPlayer)
	{
		if (iter.second->m_uiGateId == uiGateId)
			vDel.push_back(iter.first);
	}

	for (auto id : vDel)
	{
		m_mapUser.erase(id);
	}
}