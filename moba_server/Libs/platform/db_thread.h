#pragma once
//数据库线程基类
#include "util_spthread.h"
#include "util_time.h"
#include "log_mgr.h"
#include <thread>
#include <mutex>

template <class T>
class DbThread
{
public:
	DbThread(void) 
	{
	}

	virtual ~DbThread()
	{
		m_bExit = true;
		if (m_stThread.joinable())
			m_stThread.join();

		if (m_stMsgList.size() > 0)
			PopMsg();

		m_stMsgList.clear();
	}

public:
	virtual bool Init()
	{
		m_stThread = std::thread(DbThread::Run, this);
		return true;
	}

	virtual void UnInit()
	{
		m_bExit = true;
		PopMsg();
	}

	virtual void PushMsg(T& stMsg)
	{
		m_stLock.lock();
		m_stMsgList.emplace_back(stMsg);
		m_stLock.unlock();
	}

	virtual bool PopMsg()
	{
		std::list<T> stRunList;

		m_stLock.lock();
		if (m_stMsgList.empty())
		{
			m_stLock.unlock();
			return false;
		}
		//读取消息,尽快释放锁,方便写入
		stRunList.swap(m_stMsgList);
		m_stLock.unlock();

		if (stRunList.size() > 2000)
			Log_Custom("msglist", "%s, msg list too big, size:%u !", typeid(*this).name(), stRunList.size());

		//每隔2分钟记录队列数量
		if (m_stTimer.Interval() > 120)
			Log_Custom("msglist", "%s, current msg size:%u", typeid(*this).name(), stRunList.size());

		size_t uiNum = 0;
		for (auto iter = stRunList.begin(); iter != stRunList.end();)
		{
			T& stMsg = *iter;
			ExecSQL(stMsg);
			++uiNum;
			if (uiNum % 10000 == 0)
				Log_Custom("msglist", "%s, size:%u/%u", typeid(*this).name(), uiNum, (stRunList.size() + uiNum));
			iter = stRunList.erase(iter);
		}
		stRunList.clear();
		return true;
	}

	static unsigned SP_THREAD_CALL Run(void* pDbThread)
	{
		DbThread* pThis = (DbThread*)(pDbThread);
		IntervalTime stPing;
		uint32 uiSleep = 0;
		while (!pThis->m_bExit)
		{
			//每隔两分钟检测与db连接状态
			if (stPing.Interval() > 120)
				pThis->DoPing();
			//处理消息
			if (!pThis->PopMsg())
			{
				uiSleep = (uiSleep + 1) % 10 + 1;
				std::this_thread::sleep_for(std::chrono::milliseconds(uiSleep));
			}
			else
			{
				//记录日志
				if (pThis->m_stTimer.Interval() > 120)
					Log_Custom("msglist", "%s, current msg size:0", typeid(*pThis).name());
				uiSleep = 0;
			}
		}
		return 0;
	}

	virtual void DoPing() = 0;
	virtual int64 ExecSQL(T& stMsg) = 0;

protected:
	std::list<T>	m_stMsgList;
	std::mutex		m_stLock;
	std::thread		m_stThread;
	IntervalTime	m_stTimer;
	bool			m_bExit = false;
};