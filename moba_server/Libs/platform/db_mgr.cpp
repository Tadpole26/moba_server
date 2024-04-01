#include "pch.h"
#include "db_mgr.h"
#include "log_mgr.h"

DBMgr::DBMgr()
{
	m_tmrLog.Init(120);
}

DBMgr::~DBMgr()
{
	m_bExit = true;
	if (m_thread.joinable())
		m_thread.join();

	if (m_dbOpList.size() > 0)
		PopData();

	m_dbOpList.clear();
}

bool DBMgr::Init()
{
	m_thread = std::thread(DBMgr::update, this);
	return true;
}

void DBMgr::UnInit()
{
	m_bExit = true;
	PopData();
}

unsigned SP_THREAD_CALL DBMgr::update(void* pdate)
{
	DBMgr* pThis = (DBMgr*)(pdate);
	GenTimer tmPing;
	tmPing.Init(120);
	uint32 uiSleep = 0;
	while (!pThis->m_bExit)
	{
		if (tmPing.On(GetCurrTime()))
			pThis->DoPing();

		if (!pThis->PopData())
		{
			uiSleep = (uiSleep + 1) % 10 + 1;
			std::this_thread::sleep_for(std::chrono::milliseconds(uiSleep));
		}
		else
		{
			if (pThis->m_tmrLog.On(GetCurrTime()))
				Log_Custom("dblist", "%s, Size:0", typeid(*pThis).name());
			uiSleep = 0;
		}
	}
	return 0;
}

void DBMgr::PushData(sDBRequest& dbOperate)
{
	if (dbOperate.type() == eDB_None)
	{
		Log_Error("type is error, %s-%s!", dbOperate.tbl().c_str()
			, dbOperate.sectbl().name().c_str());
		return;
	}

	m_lock.lock();
	m_dbOpList.emplace_back(dbOperate);
	m_lock.unlock();
}

bool DBMgr::PopData()
{
	std::list<sDBRequest> dbRunList;

	m_lock.lock();
	if (m_dbOpList.empty())
	{
		m_lock.unlock();
		return false;
	}
	dbRunList.swap(m_dbOpList);
	m_lock.unlock();

	if (dbRunList.size() > 2000)
		Log_Custom("dblist", "%s, Size:%u too big!", typeid(*this).name(), dbRunList.size());

	if (m_tmrLog.On(GetCurrTime()))
	{
		m_uiFailedNum = 0;
		Log_Custom("dblist", "%s, Size:%u", typeid(*this).name(), dbRunList.size());
	}
	size_t i = 0;

	for (auto iter = dbRunList.begin(); iter != dbRunList.end();)
	{
		sDBRequest& item = *iter;
		if (ExecSQL(item) == -1)
			++m_uiFailedNum;
		++i;
		if (i % 10000 == 0)
			Log_Custom("dblist", "%ÊÇ£¬ Size:%u/%u", typeid(*this).name(), i, (dbRunList.size() + i));
		iter = dbRunList.erase(iter);
	}
	dbRunList.clear();
	return true;
}
