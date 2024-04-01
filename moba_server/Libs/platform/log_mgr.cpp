#include "pch.h"
#include "log_mgr.h"
#include "util_malloc.h"
#include "util_file.h"
#ifdef WIN32
#include <iostream>
#include <sys/timeb.h>
#include <time.h>
#include <windows.h>
#else 
#include <stdio.h>
#include "dirent.h"
#endif
#include <sstream>
#include <iostream>

const char* pszLogLevel[eLevelEnd] = { "Debug", "Info", "Warning", "Error", "Custom" };

sLog* NewLogItem()
{
	static size_t nMalloc = sizeof(sLog);
	return (sLog*)msg_alloc(nMalloc);
}

void FreeLogItem(sLog* pLog)
{
	if (pLog == nullptr) return;
	msg_free(pLog);
}

CLogToolMgr::CLogToolMgr()
{
	m_arreLogLevel[eTarConsole] = eLevelInfo;
	m_arreLogLevel[eTarFile] = eLevelInfo;
	setlocale(LC_ALL, "chs");
	m_nIndex = 0;
	m_nLock = 0;
	m_nQueueIndex = 0;
	m_bExit = false;
}

CLogToolMgr::~CLogToolMgr()
{
	UnInit();
}

bool CLogToolMgr::Init(size_t area, size_t index)
{
	if (m_bInit) return false;

	m_bInit =	true;
	m_nIndex =	index;
	m_nArea =	area;
	m_thread = std::thread(&CLogToolMgr::update, this);
	return true;
}

void CLogToolMgr::UnInit()
{
	if (m_bExit) return;
	m_bExit = true;
	if (m_thread.joinable()) m_thread.join();

	PrintLog(m_nQueueIndex);
	m_nQueueIndex ^= 1;
	PrintLog(m_nQueueIndex);

	for (auto itr = m_mapfile.begin(); itr != m_mapfile.end(); ++itr)
	{
		if (itr->second._file != nullptr)
			fclose(itr->second._file);
	}
}

//另一个线程从消息队列中pop消息处理
unsigned SP_THREAD_CALL CLogToolMgr::update(void* pThis)
{
	CLogToolMgr* poLogMgr = (CLogToolMgr*)pThis;
	if (!poLogMgr) return 0;
	while (!poLogMgr->m_bExit)
	{
		poLogMgr->PopLog();
	}
	return 0;
}

inline uint64_t SyncCompareExchange(volatile uint64_t* pDest, uint64_t uOldVal, uint64_t uNewVal)
{
#ifdef WIN32
	return _InterlockedCompareExchange64((volatile __int64*)pDest, (__int64)uNewVal, (__int64)uOldVal);
#else 
	return __sync_val_compare_and_swap(pDest, uOldVal, uNewVal);
#endif
}

void CLogToolMgr::PopLog()
{
	int nTempIndex = m_nQueueIndex;
	while (1 == SyncCompareExchange(&m_nLock, 0, 1));
	m_nQueueIndex ^= 1;
	m_nLock = 0;

	try
	{
		if (m_queueLogs[nTempIndex].empty())
		{
			CheckLogFileDate();
			std::this_thread::sleep_for(std::chrono::microseconds(100));
			return;
		}
		else
		{
			PrintLog(nTempIndex);
		}
	}
	catch (const std::exception&)
	{
	}
}

bool CLogToolMgr::PushLog(sLog* pLog)
{
	bool bRet = false;
	while (1 == SyncCompareExchange(&m_nLock, 0, 1));
	if (pLog->eLogType == eLevelError || m_queueLogs[m_nQueueIndex].size() < 10000)
	{
		m_queueLogs[m_nQueueIndex].push(pLog);
		bRet = true;
	}

	m_nLock = 0;
	return bRet;
}

void CLogToolMgr::SetTargetLevel(ELogTarget eTar, ELogLevel eLev)
{
	m_arreLogLevel[eTar] = eLev;
}

//打到标准输出
template <typename T>
void PrintToConsle(ELogLevel eLogType, T clr, const char* szLog, const char* szLogHead)
{
#ifdef WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), clr);
	printf("%s %s\r\n", szLogHead, szLog);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), CLR_RESET);
	fflush(stdout);
	if (eLogType == eLevelError)
	{
		FLASHWINFO info;
		info.cbSize = sizeof(FLASHWINFO);
		info.hwnd = GetConsoleWindow();
		info.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
		info.uCount = 1;
		info.dwTimeout = 0;
		FlashWindowEx(&info);
	}
#else 
	printf("%s%s%s\r\n%s", clr, szLogHead, szLog, CLR_RESET);
	fflush(stdout);
#endif
}

void CLogToolMgr::PrintLog(int nIndex)
{
	if (nIndex > 1 || nIndex < 0)
		return;

	while (!m_queueLogs[nIndex].empty())
	{
		sLog* pLog = m_queueLogs[nIndex].front();
		m_queueLogs[nIndex].pop();
		if (pLog == nullptr) continue;

		//大于打印在屏幕上的日志等级
		if (pLog->eLogType >= m_arreLogLevel[eTarConsole])
		{
			switch (pLog->eLogType)
			{
			//不同等级打印在屏幕上颜色不同
			case eLevelError:
				PrintToConsle(pLog->eLogType, CLR_RED, pLog->szBuff, pLog->szBuffHead);
				break;
			case eLevelWarning:
				PrintToConsle(pLog->eLogType, CLR_YELLOW, pLog->szBuff, pLog->szBuffHead);
				break;
			case eLevelCustom:
				PrintToConsle(pLog->eLogType, CLR_GREEN, pLog->szBuff, pLog->szBuffHead);
				break;
			default:
				PrintToConsle(pLog->eLogType, CLR_RESET, pLog->szBuff, pLog->szBuffHead);
				break;
			}
		}
		//大于输出到文件等级
		if (pLog->eLogType >= m_arreLogLevel[eTarFile])
		{
			bool bReset = false;
			for (int i = 0; i < 2; ++i)
			{
				FILE* psFile = GetLogFile(pLog, bReset);
				if (psFile)
				{
					int nRes = fprintf(psFile, "%s %s\r\n", pLog->szBuffHead, pLog->szBuff);
					if (nRes < 0)
					{
						bReset = true;
						continue;
					}
					nRes = fflush(psFile);
					if (nRes == EOF)
					{
						bReset = true;
						continue;
					}
				}
				break;
			}
		}
		FreeLogItem(pLog);
	}
}

std::string CLogToolMgr::MakeFileName(const char* szLogName)
{
	std::string strFileName = get_exec_name();
	std::string strDate = CurrTimeToString(VST_DATESIMPLE, 0);
	std::stringstream ssName;
	ssName << strDate << "_" << m_nArea << "_" << m_nIndex << "_" << strFileName << "_" << szLogName << ".log";

	std::string strPath;
#ifdef WIN32
	strPath = get_exec_path() + "\\Log\\";
	strPath = strPath + strDate;
	strPath = strPath + "\\";
#else 
	strPath = get_exec_path() + "/Log/";
	strPath = strPath + strDate;
	strPath = strPath + "/";
#endif

	create_dir(strPath.c_str());
	std::string strFull = strPath + ssName.str();
	return strFull;
}

void CLogToolMgr::CheckLogFileDate(int64 qwDate)
{
	if (qwDate == 0)
		qwDate = CurrTimeToNumber(VST_DATESIMPLE);

	time_t tCurrTime = GetCurrTime();
	if (m_qwDatePre != qwDate || tCurrTime >= (m_LastTime + 600))
	{
		m_LastTime = tCurrTime;
		for (auto& itr : m_mapfile)
		{
			if (itr.second._file != nullptr)
			{
				fclose(itr.second._file);
				itr.second._file = nullptr;
			}
		}
		m_qwDatePre = qwDate;
	}
}

//获取日志文件操作权柄
FILE* CLogToolMgr::GetLogFile(sLog* pLog, bool bReset)
{
	if (pLog == nullptr) return nullptr;

	int64 nDate = CurrTimeToNumber(VST_DATESIMPLE);
	sLogFile* pLogFile = nullptr;

	pLogFile = &(m_mapfile[pLog->szFileName]);
	if (pLogFile == nullptr) return nullptr;

	CheckLogFileDate(nDate);

	if (pLogFile->_file != nullptr)
	{
		if (bReset)
		{
			fclose(pLogFile->_file);
			pLogFile->_file = nullptr;
		}
		else
			return pLogFile->_file;
	}

	std::string strfile;
	strfile = MakeFileName(pLog->szFileName);
	FILE* psLogFile = nullptr;

#ifdef WIN32
	psLogFile = fopen(strfile.c_str(), "ab+");
#else 
	psLogFile = fopen(strfile.c_str(), "ab+e");
#endif
	if (pLogFile->_file != nullptr)
		fclose(pLogFile->_file);

	pLogFile->_data = nDate;
	pLogFile->_file = psLogFile;
	pLogFile->_filename = strfile;

	return psLogFile;
}

bool CLogToolMgr::BuildLog(ELogLevel eLL, const char* szLogName, const char* pszFile, const char* pszFunc, int nLine, sLog* pLog)
{
	if (eLL == eLevelEnd || pLog == nullptr) return false;

	if (m_arreLogLevel[eTarConsole] > eLL && m_arreLogLevel[eTarFile] > eLL)
		return false;

	if (eLL != eLevelCustom) szLogName = pszLogLevel[eLL];
	if (szLogName == nullptr) return false;

	sprintf_safe(pLog->szBuffHead, "[%s] %s: %s:%s:%d", szLogName, CurrTimeToString(VST_DATESIMPLE).c_str(), pszFile, pszFunc, nLine);
	pLog->eLogType = eLL;
	strcpy_safe(pLog->szFileName, LOG_FILEPATH_SIZE, szLogName);
	return PushLog(pLog);
}

CLogToolMgr				g_CLogMgr;
extern "C"				CLogToolMgr * GetLogMgrInstance() { return &g_CLogMgr; }


