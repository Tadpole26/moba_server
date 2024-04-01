#pragma once

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <string>
#include <chrono>
#include <stdlib.h>
#include <sys/stat.h>
#include <sstream>
#ifdef WIN32
#define snprintf _snprintf
#endif 
#ifndef WIN32
#include <unistd.h>
#include <sys/time.h>
#include <netinet/in.h>
#define memcpy_s memcpy_safe
#define sprintf_s sprintf_safe
#define fprintf_s fprintf
#define printf_s printf
#define ssacnf_s ssacnf
#define strtok_s strtok_r
#define _strlwr_s strlwr_safe
#define _strupr_s strupr_safe
#define strcat_s strcat_safe
#define fopen_s fopen
#define localtime_s(Res, TIME_T) localtime_r(TIME_T, Res)
#define gmtime_s(Res, TIME_T) gmtime_r(TIME_T, Res)
#endif

#include <ctype.h>
#include <vector>

//安全字符串拷贝函数
extern int strncpy_safe(char* szDst, size_t countofDst, const char* szSrc, size_t countofCopy);
extern int strcpy_safe(char* szDst, size_t countofDst, const char* szSrc);
//安全字符串拷贝函数,可以省略目标数组大小
template <size_t S>
inline int strncpy_safe(char(&szDst)[S], const char* szSrc, size_t countofCopy)
{
	return strncpy_safe(szDst, S, szSrc, countofCopy);
}
//安全字符串拷贝函数,可以省略目标和源数组大小
template <size_t S1, size_t S2>
inline int strncpy_safe(char(&szDst)[S1], const char(&szSrc)[S2])
{
	return strncpy_safe(szDst, S1, szSrc, S2);
}
//安全内存拷贝
extern int memcpy_safe(void* pDst, size_t countofDst, const void* pSrc, size_t countofCopy);
//安全格式化字符串函数
template <typename... Args>
inline int sprintf_safe(char* szDst, size_t countofDst, const char* szFormat, Args&&... args)
{
	int nLen = snprintf(szDst, countofDst, szFormat, std::forward<Args>(args)...);
	szDst[countofDst - 1] = 0;
	return nLen > (int)countofDst ? (int)countofDst : nLen;
}
//安全格式化字符串函数,可以省略目标数组大小
template <size_t S, typename... Args>
inline int sprintf_safe(char(&szDst)[S], const char* szFormat, Args&&... args)
{
	int nLen = snprintf(szDst, S, szFormat, std::forward<Args>(args)...);
	szDst[S - 1] = 0;
	return nLen > (int)S ? (int)S : nLen;
}
//安全转换成小写函数
extern int strlwr_safe(char* str, size_t numberOfElements);
template <size_t S>
inline int strlwr_safe(char(&str)[S]) { return strlwr_safe(str, S); }
//安全转换成大写函数
extern int strupr_safe(char* str, size_t numberOfElements);
template <size_t S>
inline int strup_safe(char(&str)[S]) { return strupr_safe(str, S); }
//安全拼接字符串
extern int strcat_safe(char* strDestination, size_t numberOfElements, const char* strSource);
//安全替换字符串
extern void str_replace(std::string& strReplaced, const std::string& origStr, const std::string& newStr);
//全部转化小写
extern std::string str_to_lower(const std::string str);
//两字符串是否相同(忽略大小写)
extern int str_cmp_ncase(const std::string strA, const std::string strB);

extern std::string trim_all(const std::string& strReplaced);
extern std::string trim_s(const std::string& text);

extern std::string check_name(const std::string& text, size_t size = 32);
extern bool check_name_valid(const std::string& text);
extern bool is_digit(std::string& text);