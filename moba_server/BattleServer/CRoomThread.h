#pragma once

#include "CBattleUser.h"
#include "map_pool.h"
#include <thread>
#include "msg_make.h"

class CRoom;
class CBattleUser;
class CTimeEvent;
class CTcpUdpLogic;

class CRoomThread : public CMsgMake
{
public:
	CRoomThread();
	~CRoomThread();

	bool Init(int iPort, int iIndex, bool bTcp = true);
	bool AddNetRoom(CRoom* pRoom);
	void PushDestroy(CRoom* pRoom, int64_t llTick);

	void Run();
	void Stop();
	void Fini();
	void OnTimer(int iTime, int64_t llTick = 0);

	int GetPort() { return m_iPort; }
	int GetIndex() { return m_iIndex; }
	int AddUserNum(int iNum);
	int DelUserNum(int iNum);
	int GetUserNum() { return m_iUserNum; }

	int64_t GetRoomId(std::string strSession);
	CRoom* GetRoom(int64_t llRoomId);

	CBattleUser* GetUser(int64_t llUserId);
	CBattleUser* CreateUser(int64_t llUserId);

	void EnterUser(CBattleUser* pBtlUser);
	void DeleteUser(CBattleUser* pBtlUser);
	void KillUser(CBattleUser* pBtlUser, int64_t llTick);
	bool PopAdd();

	void RemoveKillUser(int64_t llUserId);
	bool IsUdp() const { return m_bUdp; }

private:
	void PopRoomDestory(int64_t llTick);
	void PopKillUser(int64_t llTick);

	void AddRoom(CRoom* pRoom);
	void RemoveRoom(CRoom* pRoom);

private:
	CTcpUdpLogic*						m_pTcpUdpLogic = nullptr;
	NumTimer<140000>					m_oLogTimer;					//日志定时器
	NumTimer<33000>						m_oAddTimer;					//
	NumTimer<130>						m_oDestroyTimer;
	NumTimer<120>						m_oUserKillTimer;			
	std::mutex							m_oLockAdd;
	std::map<int64_t, CRoom*>			m_mapAdd;
	std::map<std::string, int64_t>		m_mapId;
	std::vector<CRoom*>					m_vecRoomList;
	std::map<int64_t, int64_t>			m_mapUserKill;
	std::map<int64_t, int64_t>			m_mapRoomDestroy;
	MapPool<int64_t, CBattleUser>		m_mapUser;

	int									m_iIndex = 0;
	int									m_iPort = 0;
	bool								m_bUdp = false;
	bool								m_bExit = false;
	int									m_iTmrIndex = 0;
	int									m_iUserNum = 0;
};