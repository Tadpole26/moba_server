#pragma once

//字符串处理函数
//字符串分割(允许用到的分隔符:# , : ; | -)

#include <string>
#include <vector>
#include "dtype.h"
#include <sstream>

extern size_t str_split(const std::string& strSource, char cSplit
	, std::vector<std::string>& vecRetString, bool bTrim = false
	, bool bShowError = true);
extern size_t str_split(const std::string& strSource, char cSplit
	, std::set<std::string>& vecRetString, bool bTrim = false
	, bool bShowError = true);
extern size_t str_split_float(const std::string& strSource, char cSplit
	, std::vector<double>& vecRetfloat);
template<typename T>
inline size_t str_split_num(const std::string& strSource, char cSplit, std::vector<T>& vecRetInt)
{
	static_assert(std::is_integral<T>::value, "Type T must be intergral!");
	vecRetInt.clear();
	std::vector<std::string> vecStr;
	str_split(strSource, cSplit, vecStr);
	T val = 0;
	for (const auto& itr : vecStr)
	{
		val = (T)std::stoll(itr, nullptr, 10);
		vecRetInt.push_back(val);
	}
	return vecRetInt.size();
}
//按cSplit字符分割字符串(T支持bool|char|wchar_t|short|int|long|long long)
template <typename T>
inline size_t str_split_num(const std::string& strSource, char cSplit, std::set<T>& sRetInt)
{
	static_assert(std::is_integral<T>::value, "Type T must be intergral!");
	sRetInt.clear();
	std::vector<T> vRet;
	str_split_num(strSource, cSplit, vRet);
	sRetInt.insert(vRet.begin(), vRet.end());

	return sRetInt.size();
}
//按照指定分隔符生成字符串,支持字符串和数字(T支持bool|char|wchar_t|short|int|long|long long)
template <typename Iterator>
inline std::string MakeCVStr(Iterator begin, Iterator end, char cSplit = ':')
{
	std::stringstream ss;
	bool bFirst = true;
	for (auto itr = begin; itr != end; ++itr)
	{
		if (!bFirst)
			ss << cSplit;
		ss << *itr;
		bFirst = false;
	}
	return ss.str();
}

//按照指定分隔符生成字符串,支持字符串和数字
template <typename Iterator>
inline std::string Make_Map_Str(Iterator begin, Iterator end, char cHighSplit = ';', char cLowSplit = ':')
{
	std::stringstream ss;
	bool bFirst = true;
	for (auto itr = begin; itr != end; ++itr)
	{
		if (!bFirst)
			ss << cHighSplit;
		ss << itr->first;
		ss << cLowSplit;
		ss << itr->second;

		bFirst = false;
	}
	return ss.str();
}

template <typename T>
inline std::string MakeCVStr(const T& vecInt, char cSplit = ':')
{
	return MakeCVStr(vecInt.begin(), vecInt.end(), cSplit);
}

template <typename T>
inline std::string Make_Map_Str(const T& mapData, char cHighSplit = ':', char cLowSplit = ':')
{
	return Make_Map_Str(mapData.begin(), mapData.end(), cHighSplit, cLowSplit);
}











