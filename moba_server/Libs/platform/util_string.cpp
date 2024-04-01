#include "pch.h"
#include "util_string.h"
#include <algorithm>

int strncpy_safe(char* szDst, size_t countofDst, const char* szSrc, size_t countofCopy)
{
	size_t copySize = std::min(countofDst - 1, countofCopy);
	strncpy(szDst, szSrc, copySize);
	szDst[copySize] = '\0';
	return 0;
}

int strcpy_safe(char* szDst, size_t countofDst, const char* szSrc)
{
	strncpy(szDst, szSrc, countofDst);
	szDst[countofDst - 1] = '\0';
	return 0;
}

int memcpy_safe(void* pDst, size_t countofDst, const void* pSrc, size_t countofCopy)
{
	size_t copySize = std::min(countofDst, countofCopy);
	memcpy(pDst, pSrc, copySize);
	return 0;
}

int strlwr_safe(char* str, size_t numberOfElements)
{
	char* dst = str;
	size_t i = 0;
	while (i < numberOfElements && *dst != 0) {
		*dst = tolower(*dst);
		++dst;
		++i;
	}
	return 0;
}

int strupr_safe(char* str, size_t numberOfElements)
{
	char* dst = str;
	size_t i = 0;
	while (i < numberOfElements && *dst != 0) {
		*dst = toupper(*dst);
		++dst;
		++i;
	}
	return 0;
}

int strcat_safe(char* strDestination, size_t numberOfElements, const char* strSource)
{
	strncat(strDestination, strSource, numberOfElements);
	return 0;
}

void str_replace(std::string& strReplaced, const std::string& origStr, const std::string& newStr)
{
	if (strReplaced.empty() || origStr.empty()) return;
	std::string::size_type pos = 0;
	std::string::size_type origLen = origStr.size();
	std::string::size_type newLen = newStr.size();
	while ((pos = strReplaced.find(origStr, pos)) != std::string::npos)
	{
		strReplaced.replace(pos, origLen, newStr);
		pos += newLen;
	}
}

std::string str_to_lower(const std::string str)
{
	std::string strTmp = str;
#ifdef WIN32
	std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), tolower);
#else
	std::transform(strTmp.begin(), strTmp.end(), strTmp.begin(), ::tolower);
#endif
	return strTmp;
}

int str_cmp_ncase(const std::string strA, const std::string strB)
{
	std::string str1 = str_to_lower(strA);
	std::string str2 = str_to_lower(strB);
	return str1.compare(str2);
}

extern std::string trim_s(const std::string& text)
{
	std::string dest;
	dest = text;
	if (!dest.empty())
	{
		dest.erase(0, dest.find_first_not_of('\0'));
		dest.erase(dest.find_last_not_of('\0') + 1);
		dest.erase(0, dest.find_first_not_of(" \n\r\t"));
		dest.erase(dest.find_last_not_of(" \n\r\t") + 1);
	}
	return dest;
}

extern std::string trim_all(const std::string& strReplaced)
{
	std::string strTemp = strReplaced;
	std::string strEmpty = " \n\r\t";

	strTemp.erase(0, strTemp.find_first_not_of('\0'));
	strTemp.erase(strTemp.find_last_not_of('\0') + 1);

	for (auto& itr : strEmpty)
	{
		std::string strItem;
		strItem.append(1, itr);
		str_replace(strTemp, strItem, "");
	}
	return strTemp;
}

extern std::string check_name(const std::string& text, size_t size)
{
	std::string strName;
	char* seps = (char*)",;:.!@#$%^&*=<>{}`~()/+|~!@#$%^+&\\/?|`'\" \n\r\t";
	char* token = NULL;
	char* next_token = NULL;
	char szSrc[512];
	strncpy_safe(szSrc, text.c_str(), size);

	token = strtok_s(szSrc, seps, &next_token);
	while (token != NULL)
	{
		strName.append(token);
		token = strtok_s(NULL, seps, &next_token);
	}
	return strName;
}

bool is_digit(std::string& text)
{
	if (text.empty()) return false;

	if (text.find_first_not_of("1234567890") != std::string::npos)
		return false;

	return true;
}

bool check_name_valid(const std::string& text)
{
	if (text.empty()) return false;

	if (text.find_first_of(",;:.!@#$%^&*=<>{}`~()/+|~!@#$%^+&\\/?|`'\" \n\r\t") != std::string::npos)
		return false;

	return true;
}




