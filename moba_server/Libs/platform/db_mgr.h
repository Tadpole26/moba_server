#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <functional>
#include <list>
#include "db_struct.h"
#include "global_define.h"
#include "util_time.h"

class DBMgr
{
public:
	DBMgr();
	virtual ~DBMgr();

public:
	virtual bool Init();
	virtual void UnInit();
	static unsigned SP_THREAD_CALL update(void*);
	virtual void PushData(sDBRequest& dbOperate);

	virtual bool PopData();
	virtual void DoPing() = 0;
	virtual int64 ExecSQL(sDBRequest& oRequest) = 0;

	uint32 GetFailedNum() { return m_uiFailedNum; }

protected:
	std::list<sDBRequest> m_dbOpList;
	std::mutex m_lock;
	std::thread m_thread;
	GenTimer m_tmrLog;
	bool m_bExit = false;
	uint32 m_uiFailedNum = 0;
	
};