#pragma once

#include "CRoom.h"
#include "map_pool.h"
#include <map>
#include "util_time.h"
#include "id_alloctor.h"
#include "singleton.h"

class CBattleUser;
class CRoomManager
{
public:
	CRoomManager();
	~CRoomManager();

	bool Init(int iPort, int iThreadNum, bool bTcp = false);

	CRoom* CreateRoom(int64_t llRoomId, const std::string& strSession);
	void DeleteRoom(CRoom* pRoom);
	bool HasRoom(int64_t llRoomId);
	CRoom* GetRoom(int64_t llRoomId);
	int64_t GetRoomId(const std::string& strSession);
	size_t GetRoomNum() { return m_mapSessionId.size(); }

	void OnTimer(int iTime);

	bool AddHeartRoom(CRoom* pRoom);

	void PushDestroy(int64_t llRoomId);

	int RandConv();
	void DelConv(int iId);
	
	void Stop();

private:
	CRoomThread* FindRoomThread();
	CRoomThread* AddThread(bool bTcp = true);
	bool InitAllThread();

	void PopDestory();

	MapPool<int64_t, CRoom> m_mapRoom;
	std::map<std::string, int64_t> m_mapSessionId;

	std::mutex	m_oLockDestory;
	std::map<int64_t, time_t> m_mapDestroy;

	std::map<int64_t, int64_t> m_mapUserKill;

	NumTimer<60000> m_tmrLog;

	std::vector<CRoomThread*> m_vecTmrRoom;

	int m_iMaxThread = 1;
	int m_iCurPort = 0;
	int m_iInitPort = 0;

	bool m_bTcp = false;
	std::set<int> m_setConvId;
};

#define gRoomManager Singleton<CRoomManager>::getInstance()