#include "pch.h"
#include "func_proc.h"
#include "log_mgr.h"
#include "util_file.h"
#include "util_string.h"
#include <stdio.h>

#ifndef WIN32
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/sysinfo.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#else
#include <winsock.h>
#include <process.h>
#include <shellapi.h>
#include <stdint.h>
#include <tlhelp32.h>
#include <Psapi.h>
#include <Iprtrmib.h>

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Iphlpapi.lib")
#endif

#ifdef _WIN32
//tcp
typedef struct
{
	DWORD dwState;				//连接状态
	DWORD dwLocalAddr;			//本地地址
	DWORD dwLocalPort;			//本地端口
	DWORD dwRemoteAddr;			//远程地址
	DWORD dwRemotePort;			//远程端口
	DWORD dwProcessId;			//进程标识
}MIB_TCPEXROW, *PMIB_TCPEXROW;
//udp
typedef struct
{
	DWORD dwLocalAddr;			//本地地址
	DWORD dwLocalPort;			//本地端口
	DWORD dwProcessId;			//进程标识
} MIB_UDPEXROW, * PMIB_UDPEXROW;

typedef struct
{
	DWORD dwState;				//连接状态
	DWORD dwLocdalAddr;			//本地地址
	DWORD dwLocalPort;			//本地端口
	DWORD dwRemoteAddr;			//远程地址
	DWORD dwRemotePort;			//远程端口
	DWORD dwProcessId;			//进程标识
	DWORD Unkonwn;				//待定标识
}MIB_TCPEXROW_VISTA, *PMIB_TCPEXROW_VISTA;

typedef struct
{
	DWORD dwNumEntries;
	MIB_TCPEXROW table[ANY_SIZE];
}MIB_TCPEXTABLE, *PMIB_TCPEXTABLE;

typedef struct
{
	DWORD dwNumEntries;
	MIB_TCPEXROW_VISTA table[ANY_SIZE];
}MIB_TCPEXTABLE_VISTA, * PMIB_TCPEXTABLE_VISTA;	

typedef struct
{
	DWORD dwNumEntries;
	MIB_UDPEXROW table[ANY_SIZE];
}MIB_UDPEXTABLE, *PMIB_UDPEXTABLE;


//该函数在Windows Vista以及windows 7下面有效
typedef DWORD(WINAPI* PFNInternalGetTcpTable2)(
	PMIB_TCPEXTABLE_VISTA* pTcpTable_Vista,
	HANDLE heap,
	DWORD flags);

//该函数在Windows Vista以及windows 7下面有效
typedef DWORD(WINAPI* PFNInternalGetUdpTableWithOwnerPid)(
	PMIB_UDPEXTABLE* pUdpTable,
	HANDLE heap,
	DWORD flags);

DWORD GetProcessIdByPort(DWORD dwPort, bool bTcp)
{
	HMODULE hModule = LoadLibraryW(L"iphlpapi.dll");
	if (hModule == NULL) return  0;
	if (bTcp)
	{
		//Vista或者windows 7操作系统
		PMIB_TCPEXTABLE_VISTA pTcpExTable = NULL;
		PFNInternalGetTcpTable2 pInternalGetTcpTable2 = (PFNInternalGetTcpTable2)GetProcAddress(hModule, "InternalGetTcpTable2");
		if (pInternalGetTcpTable2 == NULL)
		{
			if (pTcpExTable)
				HeapFree(GetProcessHeap(), 0, pTcpExTable);
			FreeLibrary(hModule);
			hModule = NULL;
			return 0;
		}

		if (pInternalGetTcpTable2(&pTcpExTable, GetProcessHeap(), 1))
		{
			if (pTcpExTable)
				HeapFree(GetProcessHeap(), 0, pTcpExTable);
			FreeLibrary(hModule);
			hModule = NULL;
			return 0;
		}

		for (UINT i = 0; i < pTcpExTable->dwNumEntries; i++)
		{
		//og_Custom("start", "local port:%u,%u", pTcpExTable->table[i].dwLocalPort, ntohs(0x0000FFFF & pTcpExTable->table[i].dwLocalPort));
			//过滤掉数据, 只查询我们需要的进程数据
			if (dwPort == ntohs(0x0000FFFF & pTcpExTable->table[i].dwLocalPort))
			{
				DWORD dwProcessId = pTcpExTable->table[i].dwProcessId;
				if (pTcpExTable)
					HeapFree(GetProcessHeap(), 0, pTcpExTable);
				FreeLibrary(hModule);
				hModule = NULL;
				return dwProcessId;
			}
		}

		if (pTcpExTable)
			HeapFree(GetProcessHeap(), 0, pTcpExTable);

		FreeLibrary(hModule);
		hModule = NULL;
		return 0;
	}
	else
	{
		//表明Vista或者windows 7操作系统
		PMIB_UDPEXTABLE pUdpExTable = NULL;
		PFNInternalGetUdpTableWithOwnerPid pInternalGetUdpTableWithOwnerPid;
		pInternalGetUdpTableWithOwnerPid =
			(PFNInternalGetUdpTableWithOwnerPid)GetProcAddress(hModule, "InternalGetUdpTableWithOwnerPid");
		if (pInternalGetUdpTableWithOwnerPid != NULL)
		{
			if (pInternalGetUdpTableWithOwnerPid(&pUdpExTable, GetProcessHeap(), 1))
			{
				if (pUdpExTable)
					HeapFree(GetProcessHeap(), 0, pUdpExTable);

				FreeLibrary(hModule);
				hModule = NULL;
				return 0;
			}

			for (UINT i = 0; i < pUdpExTable->dwNumEntries; i++)
			{
				//过滤掉数据,只查询我们需要的进程数据
				if (dwPort == ntohs(0x0000FFFF & pUdpExTable->table[i].dwLocalPort))
				{
					DWORD dwProcessId = pUdpExTable->table[i].dwProcessId;
					if (pUdpExTable)
						HeapFree(GetProcessHeap(), 0, pUdpExTable);
					FreeLibrary(hModule);
					hModule = NULL;
					return dwProcessId;
				}
			}
		}
		if (pUdpExTable)
			HeapFree(GetProcessHeap(), 0, pUdpExTable);

		FreeLibrary(hModule);
		hModule = NULL;
		return 0;
	}
	FreeLibrary(hModule);
	hModule = NULL;
	return -1;
}
#endif //_WIN32

uint16_t get_cpu_num()
{
	uint16_t ncpu = 1;
#ifdef WIN32
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	//处理器的数量
	ncpu = (uint16_t)sysinfo.dwNumberOfProcessors;
#ifdef _DEBUG
	return 1;
#endif	//_DEBUG
#else 
	ncpu = (uint16_t)get_nprocs();
#endif	//WIN32
	if (ncpu == 0 || ncpu == UINT16_MAX || ncpu == INT16_MAX)
		return 1;
	return ncpu;
}

std::string run_cmd_res(const std::string& strCmd)
{
	std::string strRet;
#ifndef _WIN32
	FILE* pFile = nullptr;
	char pBuff[1024];
	pFile = popen(strCmd.c_str(), "r");
	if (pFile == nullptr)
		return strRet;

	while (fgets(pBuff, sizeof(pBuff), pFile))
		strRet.append(pBuff);

	pclose(pFile);
#endif //_WIN32
	return strRet;
}

bool is_listen_port(DWORD dwPort, bool bTcp /* = true*/)
{
	if (dwPort == 0) return false;
#ifdef _WIN32
	return GetProcessIdByPort(dwPort, bTcp) != 0;
#else 
#endif
	return true;
}

std::string get_local_ip()
{
	std::string strIp = "127.0.0.1";
#ifdef WIN32
	char szHostName[260] = { 0 };
	int nRetCode;
	hostent* hostinfo = nullptr;
	nRetCode = gethostname(szHostName, sizeof(szHostName));
	if (nRetCode != 0)
	{
		Log_Error("get_local_ip is null!, default use 127.0.0.1");
		return "127.0.0.1";
	}
	hostinfo = gethostbyname(szHostName);
	if (hostinfo == nullptr)
	{
		Log_Error("get_local_ip is null!, default use 127.0.0.1");
		return "127.0.0.1";
	}

	for (size_t i = 0; hostinfo->h_addr_list[i] != nullptr; ++i)
	{
		strIp = inet_ntoa(*(struct in_addr*)(hostinfo->h_addr_list[i]));
		strIp = trim_s(strIp);
		if (strIp.compare("127.0.0.1") != 0)
			break;
	}
	return strIp;
#else 
	const size_t MAXINTERFACES = 16;
	char* ip = nullptr;
	int fd, intrface = 0;
	struct ifreq buf[MAXINTERFACES];
	struct ifconf ifc;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
	{
		ifc.ifc_len = sizeof(buf);
		ifc.ifc_buf = (caddr_t)buf;
		if (!ioctl(fd, SIOCGIFCONF, (char*)&ifc))
		{
			intrface = ifc.ifc_len / sizeof(struct ifreq);
			while (intrface-- > 0)
			{
				if (!(ioctl(fd, SIOCGIFADDR, (char*)&buf[intrface])))
				{
					ip = (inet_ntoa(((struct sockaddr_in*)(&buf[intrface].ifr_addr))->sin_addr));
					if (strcmp(ip, "127.0.0.1") != 0)
						break;
				}
			}
		}
		close(fd);
		if (ip != nullptr)
			strIp = ip;
	}
	else
	{
		Log_Error("get_local_ip is null!, default use 127.0.0.1");
	}
	return strIp;
#endif
}

std::string get_net_ip()
{
#ifdef _WIN32
	return get_local_ip();
#else 
	//curl -s ip.cn | grep -oE '([0-9]{1,3}\\.){3}[0-9]{1,3}'
	std::string strShell = get_local_path("sh/netip.sh");
	strShell = "sh " + strShell;
	const char* pCmd = strShell.c_str();
	std::string strIp = run_cmd_res(pCmd);
	if (strIp.empty())
	{
		strIp = get_local_ip();
		Log_Error("get_net_ip is null!!, default use get_local_ip:%s", strIp.c_str());
		return strIp;
	}
	else
	{
		strIp = trim_s(strIp);
		return strIp;
	}
#endif
	return "";
}

int get_pid() {
#ifdef _WIN32
	return _getpid();
#else
	return getpid();
#endif  //_WIN32
}
