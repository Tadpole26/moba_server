#include "CBattleManager.h"
#include "CRoomManager.h"
#include "log_mgr.h"

CBattleManager::CBattleManager()
{
}

CBattleManager::~CBattleManager()
{
}

bool CBattleManager::Init()
{
	return true;
}

void CBattleManager::UnInit()
{
}

void CBattleManager::OnTimer()
{
}

CBattleSession* CBattleManager::AddBattleSvr(int iServerId, int iMaxRoom)
{
	for (auto iter = m_vecBattle.begin(); iter != m_vecBattle.end(); ++iter)
	{
		if ((*iter)->GetServerId() == iServerId)
		{
			m_vecBattle.erase(iter);
			break;
		}
	}

	CBattleSession* pSession = m_oBattleMgr.AddServer(SERVER_KIND_BATTLE, iServerId);
	if (!pSession)
	{
		Log_Error("pSession is nullptr");
		return nullptr;
	}
	m_iMaxRoom += iMaxRoom;
	pSession->SetMaxRoomNum(iMaxRoom);
	m_vecBattle.push_back(pSession);

	return pSession;
}

void CBattleManager::SetBattleGroupIp(int iGroupId, std::string strIp)
{
	auto iter = m_mapBattleIp.find(iGroupId);
	if (iter != m_mapBattleIp.end())
	{
		if (iter->second.compare(strIp) == 0)
			return;
		iter->second = strIp;
	}
	m_mapBattleIp[iGroupId] = strIp;
	Log_Custom("svrip", "id:%u, ip:%s", iGroupId, strIp.c_str());
}

void CBattleManager::UnRegBattleSvr(CBattleSession* pServer)
{
	auto iter = std::find(m_vecBattle.begin(), m_vecBattle.end(), pServer);
	if (iter != m_vecBattle.end())
		m_vecBattle.erase(iter);

	if (m_iMaxRoom > pServer->GetMaxRoomNum())
		m_iMaxRoom -= pServer->GetMaxRoomNum();
	else
		m_iMaxRoom = 0;

	++m_mapBattleDiscon[pServer->GetServerId()];
	m_oBattleMgr.DelServer(SERVER_KIND_BATTLE, pServer->GetServerId());
}

int CBattleManager::GetBattleDisTs(int iServerId)
{
	return m_mapBattleDiscon[iServerId];
}

CBattleSession* CBattleManager::GetBattleServerById(int iId)
{
	return m_oBattleMgr.GetServer(SERVER_KIND_BATTLE, iId);
}

CBattleSession* CBattleManager::FindServer()
{
	if (m_vecBattle.size() == 0)
		return nullptr;
	static size_t uiIndex = 0;
	CBattleSession* pBattle = nullptr;
	size_t uiPreIndex = uiIndex;

	++uiIndex;
	uiIndex = uiIndex % m_vecBattle.size();

	pBattle = m_vecBattle[uiIndex];
	if (pBattle != nullptr && pBattle->IsValid())
		return pBattle;
	else
	{
		while (uiPreIndex != uiIndex)
		{
			++uiIndex;
			uiIndex = uiIndex % m_vecBattle.size();

			pBattle = m_vecBattle[uiIndex];
			if (pBattle != nullptr && pBattle->IsValid())
				return pBattle;
		}
	}

	return FindServerBest();
}

size_t CBattleManager::GetBattleSize()
{
	return m_vecBattle.size();
}

CBattleSession* CBattleManager::FindServerBest()
{
	int iLeftNum = 0;
	CBattleSession* pRet = nullptr;
	for (auto& pSession : m_vecBattle)
	{
		int iTempNum = pSession->GetLeftNum();
		if (iLeftNum < iTempNum)
		{
			pRet = pSession;
			iLeftNum = iTempNum;
		}
	}

	if (pRet == nullptr || !pRet->IsValid())
	{
		Log_Error("create fail room! %d/%d", gRoomManager->GetNum(), m_iMaxRoom);
		return nullptr;
	}

	return pRet;
}