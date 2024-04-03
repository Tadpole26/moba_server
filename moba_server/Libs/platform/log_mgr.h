#pragma once
#include <queue>
#include <stdint.h>
#include <string>
#include <thread>
#include <mutex>

#include "util_spthread.h"
#include "util_string.h"
#include "coutclr.h"
#include "util_time.h"

//日志等级
enum ELogLevel
{
	eLevelDebug = 0,
	eLevelInfo,
	eLevelWarning,
	eLevelError,
	eLevelCustom,
	eLevelEnd
};

enum ELogTarget
{
	eTarConsole = 1,
	eTarFile = 2
};

static const size_t LOG_BUF_SIZE = 2048;
static const size_t LOG_FILEPATH_SIZE = 64;
static const size_t LOG_BUFF_HEAD = 128;

struct sLog
{
	sLog() : eLogType(eLevelDebug) {}

	ELogLevel eLogType = eLevelDebug;
	char szFileName[LOG_FILEPATH_SIZE];
	char szBuff[LOG_BUF_SIZE];
	char szBuffHead[LOG_BUFF_HEAD];
};

extern sLog* NewLogItem();
extern void FreeLogItem(sLog* pLog);

struct sLogFile
{
	FILE*			_file = nullptr;
	int64_t			_data = 0;
	std::string		_filename;
};

template <typename T>
void PrintToConsle(ELogLevel eLogType, T clr, const char* szLog, const char* szLogHead);

class CLogToolMgr
{
public:
	CLogToolMgr();
	virtual ~CLogToolMgr();

	bool Init(size_t area, size_t index);
	void UnInit();
	static unsigned SP_THREAD_CALL update(void* pThis);
	void PopLog();
	bool PushLog(sLog* pLog);

public:
	virtual void SetTargetLevel(ELogTarget eTar, ELogLevel eLev);

	template <typename... Args>
	bool Write(ELogLevel eLL, const char* szLogName, const char* pszFile
		, const char* pszFunc, int nLine, const char* pszMsg, Args&&... args)
	{
		if (NULL == pszMsg || NULL == pszFile || NULL == pszFunc)
			return false;

		sLog* pLog = NewLogItem();
		if (pLog == nullptr) return false;

		try
		{
			sprintf_safe(pLog->szBuff, pszMsg, std::forward<Args>(args)...);
			if (!BuildLog(eLL, szLogName, pszFile, pszFunc, nLine, pLog))
			{
				FreeLogItem(pLog);
				return false;
			}
		}
		catch (const std::exception&)
		{
			FreeLogItem(pLog);
			return false;
		}
		return true;
	}

	void SetIndex(size_t index) { m_nIndex = index; }
	bool m_bExit = false;

private:
	void PrintLog(int nIndex);
	std::string MakeFileName(const char* szLogName);
	void CheckLogFileDate(int64 qwDate = 0);
	FILE* GetLogFile(sLog* pLog, bool bReset = false);
	bool BuildLog(ELogLevel eLL, const char* szLogName, const char* pszFile, const char* pszFunc, int nLine, sLog* pLog);
	
	ELogLevel						m_arreLogLevel[eTarFile + 1];
	std::map<std::string, sLogFile> m_mapfile;
	size_t							m_nIndex = 0;
	size_t							m_nArea = 0;
	int64_t							m_qwDatePre = 0;
	time_t							m_LastTime = 0;
	std::queue<sLog*>				m_queueLogs[2];
	volatile uint64_t				m_nLock;
	int								m_nQueueIndex = 0;
	bool							m_bInit = false;
	std::thread						m_thread;
};

extern "C"	CLogToolMgr * GetLogMgrInstance();

#define Log_SetTargetLevel(x, y) GetLogMgrInstance()->SetTargetLevel((x), (y));
#define Log_Debug(fmt, ...) GetLogMgrInstance()->Write(eLevelDebug, "", __FILE__, __FUNCTION__, __LINE__, (fmt), ##__VA_ARGS__);
#define Log_Info(fmt, ...) GetLogMgrInstance()->Write(eLevelInfo, "", __FILE__, __FUNCTION__, __LINE__, (fmt), ##__VA_ARGS__);
#define Log_Warning(fmt, ...) GetLogMgrInstance()->Write(eLevelWarning, "", __FILE__, __FUNCTION__, __LINE__, (fmt), ##__VA_ARGS__);
#define Log_Error(fmt, ...) GetLogMgrInstance()->Write(eLevelError, "", __FILE__, __FUNCTION__, __LINE__, (fmt), ##__VA_ARGS__);
#define Log_Custom(fname, fmt, ...) GetLogMgrInstance()->Write(eLevelCustom, (fname), __FILE__, __FUNCTION__, __LINE__, (fmt), ##__VA_ARGS__);
#define Log_Debugs(fmt) { std::stringstream ss; ss << fmt; Log_Debug(ss.str().c_str());}
#define Log_Infos(fmt) { std::stringstream ss; ss << fmt; Log_Info(ss.str().c_str());}
#define Log_Warnings(fmt) { std::stringstream ss; ss << fmt; Log_Warning(ss.str().c_str());}
#define Log_Errors(fmt) { std::stringstream ss; ss << fmt; Log_Error(ss.str().c_str());}
#define Log_Customs(fname, fmt) { std::stringstream ss; ss << fmt; Log_Custom((fname), ss.str().c_str());}

#define ERROR_EXIT(condition) \
	do \
	{\
	if (!(condition))\
		{\
			return false;\
		}\
	} while (false);


#define ERROR_LOG_EXIT0(condition) \
	do \
	{\
	if (!(condition))\
		{\
			Log_Error("error!");\
			goto Exit0;\
		}\
	} while (false)



