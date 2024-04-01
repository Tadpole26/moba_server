#include "CRoomManager.h"
#include "CBattleManager.h"
#include "log_mgr.h"

CRoomManager::CRoomManager()
{
}

CRoomManager::~CRoomManager()
{
}

bool CRoomManager::Init()
{
	return true;
}

void CRoomManager::UnInit()
{
	for (size_t i = 0; i < ROOM_FILTER_NUM; ++i)
	{
		auto& mapRoom = m_mapRooms.GetMap(i);
		auto iterRoom = mapRoom.begin();

		while (iterRoom != mapRoom.end())
		{
			CRoom* pRoom = iterRoom->second;
			pRoom->Release();
			++iterRoom;
		}
	}
	m_mapRooms.Clear();
}

void CRoomManager::OnTimer(int iTime)
{
	if (m_oTmrLog.On(iTime))
	{
		int iAll = gBattleManager->GetMaxRoom();
		Log_Custom("roomlist", "size:%u/%u, poolsize:%u, server:%u", 
			GetNum(), iAll, m_mapRooms.PoolSize(), gBattleManager->GetBattleSvrNum());
		if (iAll == 0) iAll = 1;
		double ffPoint = (double)GetNum() / (double)iAll;
		if (ffPoint >= 0.9f)			//百分之90负载开始报警
		{
			static double ffPer = 0.0f;
			if (ffPoint > ffPer)
			{
				ffPer = ffPoint;
				Log_Warning("more than 90%");
			}
		}
	}

	const map<int64_t, CRoom*>& mapRoom = m_mapRooms.TickMap();
	std::vector<int64_t> vecDel;
	for (const auto& iterRoom : mapRoom)
	{
		CRoom* pRoom = iterRoom.second;
		if (pRoom == nullptr)
		{
			vecDel.push_back(iterRoom.first);
			continue;
		}

		pRoom->OnTimer(iTime * ROOM_FILTER_NUM);

		if (pRoom->IsOvered())
			vecDel.push_back(iterRoom.first);
	}

	for (const auto& id : vecDel)
		DelRoom(id);
}

CRoom* CRoomManager::CreateRoom(int64_t llId)
{
	CRoom* pRoom = m_mapRooms.Insert(llId);
	if (pRoom == nullptr)
	{
		Log_Error("insert error");
		return nullptr;
	}
	pRoom->SetRoomId(llId);
	m_setRoomId.insert(llId);
	return pRoom;
}

CRoom* CRoomManager::GetRoom(int64_t llId)
{
	if (llId == 0)
		return nullptr;

	return m_mapRooms.Find(llId);
}

void CRoomManager::DelRoom(int64_t llId)
{
	DelWaitId(llId);
	CRoom* pRoom = m_mapRooms.Find(llId);
	if (pRoom == nullptr)
		return;

	pRoom->Release();
	m_mapRooms.Erase(llId);
	m_setRoomId.erase(llId);
}

int CRoomManager::GetNum()
{
	return (int)m_setRoomId.size();
}

void CRoomManager::AddWaitId(int64_t llId)
{
	auto iter = std::find(m_lstWaitId.begin(), m_lstWaitId.end(), llId);
	if (iter != m_lstWaitId.end())
		return;
	m_lstWaitId.push_back(llId);
}

void CRoomManager::DelWaitId(int64_t llId)
{
	if (m_lstWaitId.size() == 0)
		return;
	auto iter = std::find(m_lstWaitId.begin(), m_lstWaitId.end(), llId);
	if (iter != m_lstWaitId.end())
		m_lstWaitId.erase(iter);
}

bool CRoomManager::IsWaitId(int64_t llId)
{
	auto iter = std::find(m_lstWaitId.begin(), m_lstWaitId.end(), llId);
	return iter != m_lstWaitId.end();
}

int64_t CRoomManager::PopWaitId()
{
	if (m_lstWaitId.size() == 0)
		return INT64_MAX;
	int64_t llId = INT64_MAX;
	auto iter = m_lstWaitId.begin();
	llId = *iter;
	m_lstWaitId.pop_front();
	return llId;
}