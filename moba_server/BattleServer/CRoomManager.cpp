#include "CRoomManager.h"
#include "log_mgr.h"
#include "CRoomThread.h"
#include "util_random.h"
#include "CBattleLogic.h"
#include "func_proc.h"


CRoomManager::CRoomManager()
{
}

CRoomManager::~CRoomManager()
{
	m_mapRoom.clear();
	for (auto& iter : m_vecTmrRoom)
	{
		CRoomThread* pNet = iter;
		if (pNet)
			delete pNet;
	}
	m_vecTmrRoom.clear();
}

bool CRoomManager::Init(int iPort, int iThreadNum, bool bTcp)
{
	if (iThreadNum == 0)
		iThreadNum = 1;

	m_bTcp = bTcp;
	m_iMaxThread = iThreadNum;
	m_iCurPort = iPort;
	m_iInitPort = iPort;
	m_vecTmrRoom.reserve(iThreadNum);
	return InitAllThread();
}


CRoom* CRoomManager::CreateRoom(int64_t llRoomId, const std::string& strSession)
{
	if (llRoomId == 0) return nullptr;
	if (m_vecTmrRoom.size() == 0) return nullptr;

	CRoomThread* pThread = FindRoomThread();
	if (pThread == nullptr) return nullptr;

	CRoom* pRoom = m_mapRoom.insert(llRoomId);
	if (!pRoom)
		return nullptr;

	pRoom->SetIndex(pThread->GetIndex());
	pRoom->SetRoomId(llRoomId);
	pRoom->SetLogId(strSession);
	pRoom->SetUdp(pThread->IsUdp());

	m_mapSessionId[strSession] = llRoomId;
	pRoom->SetHostPort(m_vecTmrRoom[pRoom->GetIndex()]->GetPort());
	return pRoom;
}

void CRoomManager::DeleteRoom(CRoom* pRoom)
{
	int64_t llRoomId = pRoom->GetRoomId();
	std::map<int64_t, int>& mapConv = pRoom->GetConvList();
	for (auto& iterConv : mapConv)
		DelConv(iterConv.second);
	m_vecTmrRoom[pRoom->GetIndex()]->DelUserNum(pRoom->GetUserNum());

	pRoom->DoEnd();
	m_mapSessionId.erase(pRoom->GetLogId());
	m_mapRoom.erase(llRoomId);
}

CRoomThread* CRoomManager::FindRoomThread()
{
	CRoomThread* pRoomThread = nullptr;
	for (size_t i = 0; i < m_vecTmrRoom.size();++i)
	{
		if (m_vecTmrRoom[i]->GetUserNum() == 0)
			return m_vecTmrRoom[i];
		else if (pRoomThread == nullptr ||
			pRoomThread->GetUserNum() > m_vecTmrRoom[i]->GetUserNum())
		{
			pRoomThread = m_vecTmrRoom[i];
		}
	}
	return pRoomThread;
}

CRoomThread* CRoomManager::AddThread(bool bTcp)
{
	CRoomThread* pNet = nullptr;
	while (m_iCurPort < (m_iInitPort + m_iMaxThread))
	{
		if (!is_listen_port(m_iCurPort, bTcp))
		{
			pNet = new CRoomThread();
			if (pNet == nullptr)
			{
				++m_iCurPort;
				return nullptr;
			}
			if (!pNet->Init(m_iCurPort, (int)m_vecTmrRoom.size(), bTcp))
			{
				delete pNet;
				pNet = nullptr;
				++m_iCurPort;
				continue;
			}
			else
			{
				m_vecTmrRoom.push_back(pNet);
				++m_iCurPort;
				return pNet;
			}
		}
		else
			++m_iCurPort;
	}
	return nullptr;
}

bool CRoomManager::InitAllThread()
{
	while (m_iCurPort < (m_iInitPort + m_iMaxThread))
	{
		if (!is_listen_port(m_iCurPort, m_bTcp))
		{
			CRoomThread* pNet = new CRoomThread();
			if (pNet == nullptr)
				++m_iCurPort;

			if (!pNet->Init(m_iCurPort, (int)m_vecTmrRoom.size(), m_bTcp))
			{
				delete pNet;
				pNet = nullptr;
				++m_iCurPort;
				continue;
			}
			else
			{
				m_vecTmrRoom.push_back(pNet);
				++m_iCurPort;
			}
		}
		else
			++m_iCurPort;
	}
	return m_vecTmrRoom.size() > 0;
}

bool CRoomManager::AddHeartRoom(CRoom* pRoom)
{
	m_vecTmrRoom[pRoom->GetIndex()]->AddNetRoom(pRoom);
	m_vecTmrRoom[pRoom->GetIndex()]->AddUserNum(pRoom->GetUserNum());

	return true;
}

void CRoomManager::PushDestroy(int64_t llRoomId)
{
	m_oLockDestory.lock();
	m_mapDestroy[llRoomId] = GetCurrTime();
	m_oLockDestory.unlock();
}

bool CRoomManager::HasRoom(int64_t llRoomId)
{
	return GetRoom(llRoomId) != nullptr;
}

CRoom* CRoomManager::GetRoom(int64_t llRoomId)
{
	if (llRoomId == 0)
		return nullptr;
	return m_mapRoom.find(llRoomId);
}

int64_t CRoomManager::GetRoomId(const std::string& strSession)
{
	auto iter = m_mapSessionId.find(strSession);
	if (iter == m_mapSessionId.end())
		return 0;

	return iter->second;
}

void CRoomManager::PopDestory()
{
	std::map<int64_t, time_t> mapTemp;

	m_oLockDestory.lock();
	mapTemp.swap(m_mapDestroy);
	m_oLockDestory.unlock();

	if (mapTemp.size() == 0)
		return;

	for (auto iter = mapTemp.begin(); iter != mapTemp.end(); ++iter)
	{
		CRoom* pRoom = m_mapRoom.find(iter->first);
		if (pRoom != nullptr)
			DeleteRoom(pRoom);
	}

	//CNetFaceProxy::GetInstance->SubmitReport(false);
}

int CRoomManager::RandConv()
{
	while (true)
	{
		int iRand = rand_range(1,10000000) + gBattleLogic->GetIndex()*10000000;
		if (m_setConvId.find(iRand) == m_setConvId.end())
		{
			m_setConvId.insert(iRand);
			return iRand;
		}
	}
}

void CRoomManager::DelConv(int iId)
{
	m_setConvId.erase(iId);
}

void CRoomManager::OnTimer(int iTime)
{
	//先释放空的房间
	PopDestory();

	if (m_tmrLog.On(iTime))
	{
		Log_Custom("RoomList", "sessionsize:%u, mapsize:%u, poolsize:%u",
			m_mapSessionId.size(), m_mapRoom.size(), m_mapRoom.pool_size());
	}

}

void CRoomManager::Stop()
{
	for (auto& iter : m_vecTmrRoom)
		iter->Stop();
}
