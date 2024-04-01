#pragma once
#include <map>
#include "singleton.h"
#include "CRoom.h"
#include "map_pools.h"
#include "util_time.h"

class CRoom;
class CRoomManager
{
public:
	CRoomManager();
	~CRoomManager();

public:
	bool Init();
	void UnInit();
	void OnTimer(int iTime);

	CRoom* CreateRoom(int64_t llId);
	CRoom* GetRoom(int64_t llId);
	void DelRoom(int64_t llId);
	int GetNum();

	void AddWaitId(int64_t llId);
	void DelWaitId(int64_t llId);
	bool IsWaitId(int64_t llId);
	int64_t PopWaitId();
	size_t WaitSize() { return m_lstWaitId.size(); }

private:
	MapPools<int64_t, CRoom, ROOM_FILTER_NUM> m_mapRooms;
	std::set<int64_t> m_setRoomId;
	NumTimer<60000> m_oTmrLog;

	std::list<int64_t> m_lstWaitId; //ÅÅ¶ÓµÄid
};

#define gRoomManager Singleton<CRoomManager>::getInstance()