#include "CRoomThread.h"
#include "CBattleUser.h"
#include "CRoom.h"
#include "CTcpLogic.h"
#include "CUdpLogic.h"
#include "global_define.h"
#include "CUserManager.h"
#include "CRoomManager.h"

CRoomThread::CRoomThread()
{
}

CRoomThread::~CRoomThread()
{
	m_bExit = true;
	m_mapId.clear();
	m_vecRoomList.clear();
	m_mapUser.clear();

	if (m_pTcpUdpLogic)
	{
		m_pTcpUdpLogic->Stop();
		m_pTcpUdpLogic->Fini();
		delete m_pTcpUdpLogic;
		m_pTcpUdpLogic = nullptr; 
	}
}

bool CRoomThread::Init(int iPort, int iIndex, bool bTcp)
{
	//战斗服，单个进程的人数少,所以单线程效率可能更高点
	//并且还要保证包的顺序
	m_oLogTimer.Init((120 + iPort % 30) * 1000);

	m_bUdp = !bTcp;
	if (bTcp)
		m_pTcpUdpLogic = new CTcpLogic;
	else
		m_pTcpUdpLogic = new CUdpLogic(iPort, this);

	if (!m_pTcpUdpLogic->Init(iPort, this))
	{
		delete m_pTcpUdpLogic;
		m_pTcpUdpLogic = nullptr;
		Log_Error("logic thread new failed!");
		return false;
	}

	m_iIndex = iIndex;
	m_iPort = iPort;
	return true;
}

bool CRoomThread::AddNetRoom(CRoom* pRoom)
{
	if (pRoom == nullptr) return false;

	m_oLockAdd.lock();
	m_mapAdd[pRoom->GetRoomId()] = pRoom;
	m_oLockAdd.unlock();

	return true;
}

void CRoomThread::PushDestroy(CRoom* pRoom, int64_t llTick)
{
	m_mapRoomDestroy[pRoom->GetRoomId()] = llTick;
}

void CRoomThread::Run()
{
	if (!m_pTcpUdpLogic)
		return;
	m_pTcpUdpLogic->Run();
}

void CRoomThread::Stop()
{
	m_bExit = true;
	if (m_pTcpUdpLogic)
		m_pTcpUdpLogic->Stop();
}

void CRoomThread::Fini()
{
	m_bExit = true;
	if (m_pTcpUdpLogic)
		m_pTcpUdpLogic->Fini();
}

int CRoomThread::AddUserNum(int iNum)
{
	m_iUserNum += iNum;
	return m_iUserNum;
}

int CRoomThread::DelUserNum(int iNum)
{
	if (m_iUserNum <= iNum)
		m_iUserNum = 0;
	else
		m_iUserNum -= iNum;
	
	return m_iUserNum;
}

int64_t CRoomThread::GetRoomId(std::string strSession)
{
	auto iter = m_mapId.find(strSession);
	if (iter == m_mapId.end())
	{
		PopAdd();
		iter = m_mapId.find(strSession);
		if (iter == m_mapId.end())
			return 0;
	}
	return iter->second;
}

CRoom* CRoomThread::GetRoom(int64_t llRoomId)
{
	if (llRoomId == 0)
		return nullptr;
	for (auto& iter : m_vecRoomList)
	{
		if (iter != nullptr && iter->GetRoomId() == llRoomId)
			return iter;
	}
	return nullptr;
}

void CRoomThread::AddRoom(CRoom* pRoom)
{
	m_mapId[pRoom->GetLogId()] = pRoom->GetRoomId();
	pRoom->SetRoomNet(this);

	m_pTcpUdpLogic->AddRoom(pRoom);

	for (size_t i = 0; i < m_vecRoomList.size(); ++i)
	{
		if (m_vecRoomList[i] == nullptr)
		{
			m_vecRoomList[i] = pRoom;
			return;
		}
	}
	m_vecRoomList.push_back(pRoom);
}

void CRoomThread::RemoveRoom(CRoom* pRoom)
{
	auto& mapConv = pRoom->GetConvList();
	gUserManager->DelUser(mapConv);
	for (auto& iter : mapConv)
	{
		if (m_mapUserKill.size() == 0)
			break;

		auto iterKill = m_mapUserKill.find(iter.first);
		if (iterKill != m_mapUserKill.end())
		{
			int64_t llUserId = iterKill->first;
			m_mapUserKill.erase(iterKill);

			CBattleUser* pUser = GetUser(llUserId);
			if (pUser != nullptr && pUser->GetRoomId() == pRoom->GetRoomId())
			{
				if (m_pTcpUdpLogic)
					m_pTcpUdpLogic->KillUser(pUser);
			}
		}
	}
	m_pTcpUdpLogic->RemoveRoom(pRoom);
	m_mapId.erase(pRoom->GetLogId());
	for (size_t i = 0; i < m_vecRoomList.size(); ++i)
	{
		if (m_vecRoomList[i] == pRoom)
			m_vecRoomList[i] = nullptr;
	}
	gRoomManager->PushDestroy(pRoom->GetRoomId());
}



CBattleUser* CRoomThread::GetUser(int64_t llUserId)
{
	if (llUserId == 0)
		return nullptr;

	return m_mapUser.find(llUserId);
}

CBattleUser* CRoomThread::CreateUser(int64_t llUserId)
{
	CBattleUser* pUser = m_mapUser.insert(llUserId);
	if (pUser != nullptr)
		gUserManager->AddUser(pUser->GetUserId(), pUser->GetConvId());
	return pUser;
}

void CRoomThread::EnterUser(CBattleUser* pBtlUser)
{
	RemoveKillUser(pBtlUser->GetUserId());
	m_pTcpUdpLogic->EnterUser(pBtlUser);
}

void CRoomThread::DeleteUser(CBattleUser* pBtlUser)
{
	RemoveKillUser(pBtlUser->GetUserId());

	m_pTcpUdpLogic->DeleteUser(pBtlUser);
	gUserManager->DelUser(pBtlUser->GetUserId(), pBtlUser->GetConvId());
	m_mapUser.erase(pBtlUser->GetUserId());
}

void CRoomThread::KillUser(CBattleUser* pBtlUser, int64_t llTick)
{
	m_mapUserKill[pBtlUser->GetUserId()] = llTick;
}

bool CRoomThread::PopAdd()
{
	std::map<int64_t, CRoom*> mapAdd;
	m_oLockAdd.lock();
	if (m_mapAdd.size() == 0)
	{
		m_oLockAdd.unlock();
		return false;
	}

	mapAdd.swap(m_mapAdd);
	m_oLockAdd.unlock();
	if (mapAdd.size() == 0)
		return false;

	CRoom* pRoom = nullptr;
	for (auto& iter : mapAdd)
	{
		pRoom = iter.second;
		if (m_mapId.find(pRoom->GetLogId()) != m_mapId.end())
			continue;
		AddRoom(pRoom);
	}
	return true;
}

void CRoomThread::RemoveKillUser(int64_t llUserId)
{
	if (m_mapUserKill.size() > 0)
	{
		auto iter = m_mapUserKill.find(llUserId);
		if (iter != m_mapUserKill.end())
			m_mapUserKill.erase(iter);
	}
}

void CRoomThread::OnTimer(int iTime, int64_t llTick)
{
	if (llTick == 0)
		llTick = GetMillSec();

	if (m_oUserKillTimer.On(iTime))
		PopKillUser(llTick);

	if (m_oDestroyTimer.On(iTime))
		PopRoomDestory(llTick);

	static size_t uiPreNum = NET_FRAME_TIME / iTime;
	m_pTcpUdpLogic->Tick(llTick);

	if (m_vecRoomList.size() > 0)
	{
		for (size_t j = 0; j < MAX_HEART_ROOM; ++j)
		{
			size_t uiIndex = uiPreNum * j + m_iTmrIndex;
			if (uiIndex >= m_vecRoomList.size())
				break;

			if (m_vecRoomList[uiIndex] != nullptr)
				m_vecRoomList[uiIndex]->OnTimer(llTick);
		}
	}

	m_iTmrIndex = (m_iTmrIndex + 1) % uiPreNum;

	if (m_oAddTimer.On(iTime))
		PopAdd();

	if (m_oLogTimer.On(iTime))
	{
		Log_Custom("threadlist", "port:%d, usersize:%u, poolsize:%u, roomsize:%u", m_iPort, 
			m_mapUser.size(), m_mapUser.pool_size(), m_mapId.size());
	}
}

void CRoomThread::PopRoomDestory(int64_t llTick)
{
	if (m_mapRoomDestroy.size() == 0)
		return;

	for (auto iter = m_mapRoomDestroy.begin(); iter != m_mapRoomDestroy.end();)
	{
		if (iter->second == 0 || (iter->second + WAIT_DELETE_MILLSEC) <= llTick)
		{

		}
	}
}
void CRoomThread::PopKillUser(int64_t llTick)
{
	if (m_mapUserKill.size() == 0)
		return;

	for (auto iter = m_mapUserKill.begin(); iter != m_mapUserKill.end();)
	{
		if (iter->second == 0 || (iter->second + WAIT_DELETE_MILLSEC) <= llTick)
		{
			int64_t llUserId = iter->first;
			iter = m_mapUserKill.erase(iter);

			CBattleUser* pUser = GetUser(llUserId);
			if (pUser && m_pTcpUdpLogic)
				m_pTcpUdpLogic->KillUser(pUser);
		}
		else
			++iter;
	}
}