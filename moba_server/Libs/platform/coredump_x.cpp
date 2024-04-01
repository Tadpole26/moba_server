#include "pch.h"
#include "coredump_x.h"
#include "log_mgr.h"
#include "util_file.h"
#include "util_time.h"
#include "call_stack.h"
#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
#else 
#endif

#define CALLSTACK_BUF_SIZE 0x64000
fn_proc_stop g_fnStop = nullptr;

#ifdef WIN32
int WINAPI ConsoleHandlerRoutine(unsigned long dwCtrlType)
{
	if (g_fnStop != nullptr)
		g_fnStop();
	return true;
}

bool SetConsoleInfo(fn_proc_stop fnExit)
{
	g_fnStop = fnExit;
	HANDLE		hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD		BufferSize = { 108, 320 };
	SMALL_RECT WinRect = { 0, 0, BufferSize.X - 1, 27 };

	SetConsoleScreenBufferSize(hConsole, BufferSize);
	SetConsoleWindowInfo(hConsole, true, &WinRect);

	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	SetConsoleCtrlHandler(&ConsoleHandlerRoutine, true);
	return true;
}
#else	//Linux
#include <sys/resource.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void OnQuitSignal(int nSignal)
{
	if (g_fnStop != nullptr)
		g_fnStop();
}

bool SetConsoleInfo(fn_proc_stop fnExit)
{
	g_fnStop = fnExit;
	bool	bResult = false;
	int		nRetCode = 0;
	int		nNullFile = -1;
	rlimit	ResLimit;

	ResLimit.rlim_cur = UINT16_MAX;
	ResLimit.rlim_max = UINT16_MAX;

	nRetCode = setrlimit(RLIMIT_NOFILE, &ResLimit);
	if (nRetCode != 0)
	{
		Log_Error("error!");
		goto Exit0;
	}

	nRetCode = getrlimit(RLIMIT_CORE, &ResLimit);
	if (nRetCode != 0)
	{
		Log_Error("error");
		goto Exit0;
	}

	ResLimit.rlim_cur = ResLimit.rlim_max;
	nRetCode = setrlimit(RLIMIT_CORE, &ResLimit);
	if (nRetCode != 0)
	{
		Log_Error("error");
		goto Exit0;
	}

	signal(SIGINT, OnQuitSignal);
	signal(SIGQUIT, OnQuitSignal);
	signal(SIGTERM, OnQuitSignal);
	bResult = true;
Exit0:
	if (nNullFile != -1)
	{
		close(nNullFile);
		nNullFile = -1;
	}
	return bResult;
}
#endif	//WIN32

void WriteCallList()
{
	char szDumpFileName[256] = { 0 };
	char szBuf[CALLSTACK_BUF_SIZE];
	TCallStack::Stack(szBuf, sizeof szBuf);

	FILE* psLogFile = nullptr;
#ifdef WIN32
	sprintf_s(szDumpFileName, sizeof(szDumpFileName), "%s\\%s.%s.%u.log",
		get_exec_path().c_str(),
		get_exec_name().c_str(),
		CurrTimeToString(VST_ALLSIMPLE, std::time(nullptr)).c_str(),
		GetCurrentProcessId());
	psLogFile = fopen(szDumpFileName, "ab+");
#else 
	sprintf_s(szDumpFileName, sizeof(szDumpFileName), "%s/%s.%s.%u.log",
		get_exec_path().c_str(),
		get_exec_name().c_str(),
		CurrTimeToString(VST_ALLSIMPLE, std::time(nullptr)).c_str(),
		getpid());
	psLogFile = fopen(szDumpFileName, "ab+e");
#endif 
	if (psLogFile != nullptr)
	{
		try
		{
			fprintf(psLogFile, "%s", szBuf);
		}
		catch (const std::exception&)
		{
		}
		fflush(psLogFile);
		fclose(psLogFile);
	}
}

#ifdef WIN32
#include <windows.h>
#include <dbghelp.h>
#include <stdio.h>
#pragma comment(lib, "dbghelp.lib")

LONG WINAPI CoreDump_Handler(struct _EXCEPTION_POINTERS* pExceptionPointers)
{
	WriteCallList();
	SetErrorMode(SEM_NOGPFAULTERRORBOX);
	char szDumpFileName[256] = { 0 };
	sprintf_s(szDumpFileName, sizeof(szDumpFileName), "%s\\%s.%s.%u.dmp",
		get_exec_path().c_str(),
		get_exec_name().c_str(),
		CurrTimeToString(VST_ALLSIMPLE, std::time(nullptr)).c_str(),
		GetCurrentProcessId());

	HANDLE hDumpFile = CreateFile(szDumpFileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_WRITE | FILE_SHARE_READ,
		0,
		CREATE_ALWAYS,
		0,
		0);
	if (hDumpFile != INVALID_HANDLE_VALUE)
	{
		MINIDUMP_EXCEPTION_INFORMATION ExpParam;
		ExpParam.ThreadId = GetCurrentThreadId();
		ExpParam.ExceptionPointers = pExceptionPointers;
		ExpParam.ClientPointers = TRUE;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile,
			MiniDumpNormal,
			&ExpParam,
			NULL,
			NULL);
		CloseHandle(hDumpFile);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}
#else
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
void CoreDump_Handler(int i)
{
	WriteCallList();
	abort();
}
#endif

void InstallCoreDumper()
{
#ifdef WIN32
	SetUnhandledExceptionFilter(CoreDump_Handler);
#else 
	signal(SIGSEGV, CoreDump_Handler);
	signal(SIGFPE, CoreDump_Handler);
	signal(SIGILL, CoreDump_Handler);
#endif 
}
