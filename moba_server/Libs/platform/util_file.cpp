#include "pch.h"
#include "util_file.h"
#include <stdio.h>
#ifndef WIN32
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#else
#include <windows.h>
#include <process.h>
#include <stdint.h>
#endif


int create_dir(const char* src)
{
	char sz[1024];
	memcpy_s(sz, 1024, src, strlen(src));

	int i = 0;
	int iRet = 0;
	int iLen = (int)strlen(src);
	if (sz[iLen - 1] != '\\' && sz[iLen - 1] != '/')
	{
		sz[iLen] = '/';
		sz[iLen + 1] = '\0';
	}
	for (i = 1; i < iLen + 1; ++i)
	{
		if (sz[i] == '\\' || sz[i] == '/')
		{
			sz[i] = '\0';
			iRet = ACCESS(sz, 0);
			if (iRet != 0)
			{
				iRet = MKDIR(sz);
				if (iRet != 0)
					return -1;
			}
			sz[i] = '/';
		}
	}
	return 0;
}

std::string get_full_name()
{
	static std::string strRet;
	if (!strRet.empty())
		return strRet;

	char szpath[1024];

#ifndef WIN32
	char szlink[1024];
	sprintf(szlink, "/proc/%d/exe", getpid());
	int32_t rslt = readlink(szlink, szpath, sizeof(szpath));
	if (rslt < 0) return "";
	szpath[rslt] = '\0';
	strRet.append(szpath, rslt);
#else
	uint32_t len = GetModuleFileName(NULL, szpath, sizeof(szpath) - 1);
	strRet.append(szpath, len);
#endif
	return strRet;
}

std::string get_exec_name()
{
	static std::string strRet;
	if (!strRet.empty())
		return strRet;

	std::string strTemp, strFullPath;
	strTemp = get_full_name();

	const char* szpath = strTemp.c_str();
	const char* path_end = nullptr;

#ifndef WIN32
	path_end = strrchr(szpath, '/');
#else
	path_end = strrchr(szpath, '\\');
#endif
	if (path_end == nullptr) return "";
	strFullPath.append(path_end + 1, strlen(path_end + 1));

#ifdef WIN32
	int nPos = (int)strFullPath.find_last_of('.');
	if (nPos != -1)
		strRet = strFullPath.substr(0, nPos);
	else
		strRet = strFullPath;
#else 
	strRet = strFullPath;
#endif
	return strRet;
}

std::string get_exec_path()
{
	static std::string strRet;
	if (!strRet.empty())
		return strRet;

	std::string strTemp;
	strTemp = get_full_name();
	const char* szpath = strTemp.c_str();

	const char* path_end = nullptr;
#ifndef WIN32
	path_end = strrchr(szpath, '/');
#else 
	path_end = strrchr(szpath, '\\');
#endif

	if (path_end == nullptr) return "";
	strRet.append(szpath, (path_end - szpath));
	return strRet;
}

//判断文件是否存在
bool exsit_file(const std::string& strFileName)
{
#ifdef WIN32
	if (_access(strFileName.c_str(), 0) == -1)
#else
	if (access(strFileName.c_str(), 0) == -1)
#endif //_WIN32
		return false;
	return true;
}

std::string get_local_path(const char* szFile)
{
	std::string ret = get_exec_path();
	ret += "/";
	ret += szFile;
	return ret;
}