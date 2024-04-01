#include "CUserManager.h"
#include "global_define.h"
#include "util_time.h"
#include "CGateLogic.h"
#include "CServerNetface.h"
#include "CSysNetface.h"
#include "PlayerProxy.h"
#include "util_string.h"

CUserManager::CUserManager()
{
	m_stOnLineUp.Init(60000);
	m_stLineUp.Init(10);
	m_stRandNotice.Init(1000);
}

CUserManager::~CUserManager()
{
	//正在游戏中的玩家
	for (const auto& iter : m_mapInGame)
	{
		m_pUserPool->Free(iter.second);
	}
	//检验中的玩家
	for (const auto& iter : m_mapChecking)
	{
		m_pUserPool->Free(iter.second);
	}
	for (const auto& iter : m_mapCheckingPwd)
	{
		m_pUserPool->Free(iter.second);
	}
	for (const auto& iter : m_lstWaitCheckPwd)
	{
		m_pUserPool->Free(iter);
	}
	m_mapInGame.clear();
	m_mapChecking.clear();
	m_mapCheckingPwd.clear();
	m_lstWaitCheckPwd.clear();

	if (m_pUserPool)
	{
		m_pUserPool->Destroy();
		SAFE_DELETE(m_pUserPool);
	}
}

bool CUserManager::Init()
{
	m_pUserPool = new ObjectsPool<CUser>;
	if (!m_pUserPool->Create(PLAYER_INIT_MAX))
		return false;
	
	m_mapInGame.clear();
	m_mapChecking.clear();
	m_mapCheckingPwd.clear();

	m_uiPreSecLogin = gGateLogic->m_oConstConfig.m_uiCheckMax;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CUser* CUserManager::AddNewWaitCheckPlayer(int64 llUid)
{
	CUser* pNewPlayer = m_pUserPool->Alloc();
	if (pNewPlayer == nullptr) return nullptr;

	pNewPlayer->m_llUid = llUid;
	pNewPlayer->UpdateStatus(CUser::eUserStatus::STATUS_ReadyToCheck);
	return pNewPlayer;
}

void CUserManager::AddWaitCheckPlayer(CUser* pPlayer)
{
	m_lstWaitCheckPwd.push_back(pPlayer);
}

void CUserManager::DelWiatCheckPlayer(int64 llUid)
{
	auto iter = std::find_if(m_lstWaitCheckPwd.begin(), m_lstWaitCheckPwd.end(),
		[llUid](CUser* pPlayer) { return llUid == pPlayer->m_llUid; });

	if (iter != m_lstWaitCheckPwd.end())
		m_lstWaitCheckPwd.erase(iter);
}


CUser* CUserManager::GetWaitCheckPlayer(int64 llUid)
{
	auto iter = std::find_if(m_lstWaitCheckPwd.begin(), m_lstWaitCheckPwd.end(),
		[llUid](CUser* pPlayer) { return llUid == pPlayer->m_llUid; });

	if (iter != m_lstWaitCheckPwd.end())
		return (CUser*)(*iter);
	else
		return nullptr;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CUser* CUserManager::GetCheckingPwdPlayer(int64 llUid)
{
	auto iter = m_mapCheckingPwd.find(llUid);
	if (iter != m_mapCheckingPwd.end())
		return iter->second;
	else
		return nullptr;
}

void CUserManager::DelCheckingPwdPlayer(int64 llUid)
{
	auto iter = m_mapCheckingPwd.find(llUid);
	if (iter != m_mapCheckingPwd.end())
		m_mapCheckingPwd.erase(iter);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CUser* CUserManager::GetCheckingPlayer(int64 llUid)
{
	auto iter = m_mapChecking.find(llUid);
	if (iter != m_mapChecking.end())
		return iter->second;
	else
		return nullptr;
}

void CUserManager::AddCheckingUser(int64_t llUid)
{
	CUser* pUser = GetCheckingPwdPlayer(llUid);
	if (pUser == nullptr)
		return;

	DelCheckingPwdPlayer(llUid);
	m_mapChecking.emplace(pUser->m_llUid, pUser);
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CUser* CUserManager::AddInGamePlayer(int64 llUid)
{
	//正常登录(包括没有创建角色时)
	PLAYER_MAP::iterator iter = m_mapChecking.find(llUid);
	if (iter != m_mapChecking.end())
	{
		CUser* pPlayer = (CUser*)(iter->second);
		pPlayer->UpdateStatus(CUser::eUserStatus::STATUS_CheckedOK);
		m_mapInGame.insert(std::make_pair(llUid, iter->second));
		m_mapChecking.erase(iter);
		return pPlayer;
	}
	//创建角色时
	PLAYER_MAP::iterator it = m_mapInGame.find(llUid);
	if (it != m_mapInGame.end())
		return (CUser*)(it->second);
	return nullptr;
}

void  CUserManager::DelGamePlayer(int64 llUid)
{
	PLAYER_MAP::iterator iter = m_mapInGame.find(llUid);
	if (iter != m_mapInGame.end() && iter->second)
	{
		CUser* pPlayer = iter->second;
		const auto it = m_mapThreadToConn.find(pPlayer->get_toid());
		if (it != m_mapThreadToConn.end())
		{
			auto itDel = std::find(it->second.begin(), it->second.end(), pPlayer->get_coid());
			if (itDel != it->second.end())
				it->second.erase(itDel);
		}
		m_pUserPool->Free(iter->second);
		m_mapInGame.erase(iter);
		return;
	}

	iter = m_mapChecking.find(llUid);
	if (iter != m_mapChecking.end() && iter->second)
	{
		m_pUserPool->Free(iter->second);
		m_mapChecking.erase(iter);
		return;
	}

	auto itList = std::find_if(m_lstWaitCheckPwd.begin(), m_lstWaitCheckPwd.end(),
		[llUid](CUser* pPlayer) { return llUid == pPlayer->m_llUid; });
	if (itList != m_lstWaitCheckPwd.end() && *itList)
	{
		m_pUserPool->Free(iter->second);
		m_lstWaitCheckPwd.erase(itList);
		return;
	}

	iter = m_mapCheckingPwd.find(llUid);
	if (iter != m_mapCheckingPwd.end() && iter->second)
	{
		m_pUserPool->Free(iter->second);
		m_mapCheckingPwd.erase(iter);
		return;
	}
}

CUser* CUserManager::GetInGamePlayer(int64 llUid)
{
	PLAYER_MAP::iterator iter = m_mapInGame.find(llUid);
	if (iter != m_mapInGame.end() && iter->second)
		return iter->second;
	else
		return nullptr;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CUserManager::OnTimer(uint32 uiDwTm)
{
	//正在验证密码
	OnTimerCheckingPwd();
	//正在从游戏服加载数据
	OnTimerChecking();
	//等待密码验证
	OnTimerCheckPwd();

	if (m_stRandNotice.On(uiDwTm))
		OnTimerRandNotice();

	//连接gameserver成功
	if (m_stOnLineUp.On(uiDwTm) && m_bLoadFinish)
	{
		gGateLogic->m_pSysNetface->UpdateOnlinePlayerNum();
		Log_Custom("userlist", "ingame:%u, waitcheckPwd:%u, cheking:%u, checkingpwd:%u, cdache:%d, maxmsg:%u",
			m_mapInGame.size(), m_lstWaitCheckPwd.size(), m_mapChecking.size(),
			m_mapCheckingPwd.size(), m_pUserPool->GetPoolSize(), gGateLogic->GetMaxSend());
	}
}


//3s检测一次要登录玩家
void CUserManager::OnTimerCheckPwd()
{
	time_t llCurTime = GetCurrTime();
	uint32 m_uiWaitCheck = gGateLogic->m_oConstConfig.m_uiWaitCheck;
	
	//审视待验证队列
	auto itWaitCheck = m_lstWaitCheckPwd.begin();
	uint32 uiCount = 0;
	while (itWaitCheck != m_lstWaitCheckPwd.end())
	{
		CUser* pPlayer = (CUser*)(*itWaitCheck);
		//是否超时
		if (llCurTime - pPlayer->m_stStat.m_llRecTime > m_uiWaitCheck)
		{
			itWaitCheck++;
			gGateLogic->m_pClientLIF->OnDisconnect(pPlayer);
			Log_Error("login wait so long, user name=%s, accountid=%lld", pPlayer->m_strAccName.c_str(), pPlayer->m_llUid);
			continue;
		}

		//已经达到游戏服上限,登录等待上线
		if (IsFull() || uiCount >= gGateLogic->m_oConstConfig.m_uiCheckMax)
		{
			itWaitCheck++;
			continue;
		}

		CPlayerProxy::CheckingSessionFromLoginSvr(pPlayer);
		m_mapCheckingPwd.emplace(pPlayer->m_llUid, pPlayer);
		pPlayer->UpdateStatus(CUser::eUserStatus::STATUS_CheckingFromLoginSrv);
		itWaitCheck = m_lstWaitCheckPwd.erase(itWaitCheck);
		//增加每秒的登录数量统计,用于预估时间
		AddSecLogin();	

		++uiCount;
	}

	//超负载断线提醒
	static int32 iTimes = 0;
	static time_t llPre = GetCurrTime();
	//百分之90负载连续36000次报警一次
	if ((m_mapInGame.size() + m_mapChecking.size()) >= gGateLogic->m_oConstConfig.m_uiLimit*0.9f && (iTimes % 36000 == 0))
	{
		char szLog[4096];
		sprintf_safe(szLog, "ingame:%u, waitcheck:%u,  checking:%u, checkingpwd:%u, limit:%u, checkmax:%u, times:%d",
			m_mapInGame.size(), m_lstWaitCheckPwd.size(), m_mapChecking.size(), m_mapCheckingPwd.size(),
			gGateLogic->m_oConstConfig.m_uiLimit, gGateLogic->m_oConstConfig.m_uiCheckMax, iTimes);
		//GATE_LOGIC_INS->SendWChatMsg("Gate", "Gate", szLog);
		iTimes = 0;
	}

	if (IsSameDay(llPre, GetCurrTime()))
		++iTimes;
	else
	{
		iTimes = 0;
		llPre = GetCurrTime();
	}

}

//更新用户登录列表
void CUserManager::OnTimerCheckingPwd()
{
	time_t llCurTime = GetCurrTime();
	uint32 uiTimeOut = gGateLogic->m_oConstConfig.m_uiTimeout;

	PLAYER_MAP::iterator itCheckingPwd = m_mapCheckingPwd.begin();
	while (itCheckingPwd != m_mapCheckingPwd.end())
	{
		CUser* pPlayer = (CUser*)(itCheckingPwd->second);
		//等待验证超时
		if (llCurTime - pPlayer->m_stStat.m_llRecTime > uiTimeOut)
		{
			itCheckingPwd++;
			gGateLogic->m_pClientLIF->OnDisconnect(pPlayer);
			continue;
		}
		itCheckingPwd++;
	}
}

//更新用户登录列表
void CUserManager::OnTimerChecking()
{
	time_t llCurTime = GetCurrTime();
	uint32 uiTimeOut = gGateLogic->m_oConstConfig.m_uiTimeout;

	PLAYER_MAP::iterator itChecking = m_mapChecking.begin();
	while (itChecking != m_mapChecking.end())
	{
		CUser* pPlayer = (CUser*)(itChecking->second);
		//等待验证超时
		if (llCurTime - pPlayer->m_stStat.m_llRecTime > uiTimeOut)
		{
			itChecking++;
			gGateLogic->m_pClientLIF->OnDisconnect(pPlayer);
			continue;
		}
		itChecking++;
	}
}

void CUserManager::OnTimerRandNotice()
{
	time_t llCurTime = GetCurrTime();
	for (auto iter = m_mapRandNotice.begin(); iter != m_mapRandNotice.end();)
	{
		rand_notice_t& stNoitem = iter->second;
		if ((stNoitem.m_llLast + stNoitem.m_uiInterval) >= llCurTime)
		{
			const tagMsgHead* pNetMsgHead = reinterpret_cast<const tagMsgHead*>(stNoitem.m_strMsg.c_str());
			if (pNetMsgHead == nullptr)
			{
				Log_Error("pNetMsgHead is nullptr");
			}
			else
			{
				SendToAllPlayer(pNetMsgHead);
			}
			iter = m_mapRandNotice.erase(iter);
		}
		else
			++iter;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CUserManager::PushThreadConn(CUser* pPlayer)
{
	auto iter = m_mapThreadToConn.find(pPlayer->get_toid());
	if (iter == m_mapThreadToConn.end())
		m_mapThreadToConn.emplace(pPlayer->get_toid(), std::vector<uint16>{pPlayer->get_coid()});
	else
		iter->second.push_back(pPlayer->get_coid());
}

void CUserManager::SendToAllPlayer(const std::string& strMsg, msg_id_t usModule,
	msg_id_t usCmd, std::map<uint16, std::vector<uint16>>& stThreadToConnMap)
{
	static CMsgMake stMake;
	tagMsgHead* pMyNetMsgHead = stMake.MakeHeadMsg(strMsg, usModule, usCmd, 0);

	for (auto& value : m_mapThreadToConn)
	{
		std::vector<uint16>& uiConnVec = value.second;
		if (uiConnVec.size() == 0) continue;

		uint16* pConn = &uiConnVec[0];
		my_multicast_conn_msg(gGateLogic->m_pLogic->Dispatcher(),
			value.first, pConn, (uint16_t)uiConnVec.size(), pMyNetMsgHead);
	}
}

void CUserManager::SendToAllPlayer(const tagMsgHead* pMyNetMsgHead)
{
	for (auto& value : m_mapThreadToConn)
	{
		std::vector<uint16>& uiConnVec = value.second;
		if (uiConnVec.size() == 0) continue;

		uint16* pConn = &uiConnVec[0];
		my_multicast_conn_msg(gGateLogic->m_pLogic->Dispatcher(),
			value.first, pConn, (uint16_t)uiConnVec.size(), pMyNetMsgHead);
	}
}

void CUserManager::SendNoticeAllPlayer(const std::string& stMsg, msg_id_t usModule, msg_id_t usCmd, uint32 uiInterval)
{
	auto& stNotitem = m_mapRandNotice[MAKE_UINT64(usModule, usCmd)];
	stNotitem.m_strMsg = stMsg;
	stNotitem.m_usModule = usModule;
	stNotitem.m_usCmd = usCmd;
	stNotitem.m_uiInterval = uiInterval;
}


bool CUserManager::SendToPlayer(int64 llUid, const tagMsgHead* pNetMsgHead)
{
	if (llUid == 0 || pNetMsgHead == nullptr)
	{
		Log_Error("player Id: %lld!", llUid);
		return false;
	}

	CUser* pPlayer = gGateLogic->m_oUserMgr.GetInGamePlayer(llUid);
	if (pPlayer == nullptr)
	{
		Log_Warning("pPlayer is nullptr, player id:%lld, nodule:%u, cmd:%u!", 
			llUid, pNetMsgHead->usModuleId, pNetMsgHead->uiCmdId);
		return false;
	}

	return pPlayer->Send(pNetMsgHead);
}


void CUserManager::ResetSecLogin()
{
	if (m_stLineUp.On(10))
	{
		if (IsFull() && m_uiSecLogin == 0)
			return;

		m_uiPreSecLogin = m_uiSecLogin;
		if (m_uiPreSecLogin == 0)
			m_uiPreSecLogin = gGateLogic->m_oConstConfig.m_uiCheckMax;

		m_uiSecLogin = 0;
		m_llPreLogin = GetCurrTime();
	}
}

void CUserManager::AddSecLogin()
{
	++m_uiSecLogin;
}

void CUserManager::OnTimerLineUp()
{
	size_t uiSum = m_lstWaitCheckPwd.size();
	time_t llCurTime = GetCurrTime();
	uint32 uiPos = 0;

	if (m_uiPreSecLogin == 0)
	{
		m_uiPreSecLogin = gGateLogic->m_oConstConfig.m_uiCheckMax;
		m_llPreLogin = GetCurrTime();
	}

	//uint32 uiWaitBase = (uint32)(60 + llCurTime - m_llPreLogin);
	if (IsFull() || uiSum > gGateLogic->m_oConstConfig.m_uiCheckMax)
	{
		uint32 uiCurIndex = 0;
		for (auto& player : m_lstWaitCheckPwd)
		{
			++uiCurIndex;
			if (!player || llCurTime < player->m_llNextLineUpPushTime)
			{
				++uiPos;
				continue;
			}
			player->m_llNextLineUpPushTime = llCurTime + gGateLogic->m_oConstConfig.m_uiNotifyInterval;
		}
	}
}

bool CUserManager::IsFull()
{
	return (m_mapInGame.size() + m_mapChecking.size() + m_mapCheckingPwd.size()) >= gGateLogic->m_oConstConfig.m_uiLimit;
}

bool CUserManager::ReLoginUser(CUser* pPlayer)
{
	CPlayerProxy::CheckingSessionFromLoginSvr(pPlayer);
	m_mapCheckingPwd.emplace(pPlayer->m_llUid, pPlayer);
	pPlayer->UpdateStatus(CUser::eUserStatus::STATUS_CheckingFromLoginSrv);
	return true;
}


CUser* CUserManager::GetPlayer(int64 llUid)
{
	CUser* pPlayer = GetWaitCheckPlayer(llUid);
	if (pPlayer) return pPlayer;

	pPlayer = GetCheckingPwdPlayer(llUid);
	if (pPlayer) return pPlayer;

	pPlayer = GetCheckingPlayer(llUid);
	if (pPlayer) return pPlayer;

	pPlayer = GetInGamePlayer(llUid);
	if (pPlayer) return pPlayer;

	return nullptr;
}