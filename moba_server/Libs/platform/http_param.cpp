#include "pch.h"
#include "http_param.h"
#include <iostream>

CHttpParam::CHttpParam()
{
}

CHttpParam::~CHttpParam()
{
}

void CHttpParam::AddParam(std::string strIndex, std::string strValue)
{
	std::string str = strIndex + "=" + strValue;
	m_vecParam.push_back(str);
	m_mapParam.insert(std::make_pair(strIndex, strValue));
}

void CHttpParam::AddParam(std::string strIndex, int64_t value)
{
	std::string str = strIndex + "=";
	std::string strValue = std::to_string(value);
	str = str + std::to_string(value);
	m_vecParam.push_back(str);
	m_mapParam.insert(std::make_pair(strIndex, strValue));
}

std::string CHttpParam::GetParam(std::string strIndex)
{
	auto iter = m_mapParam.find(strIndex);
	if (iter != m_mapParam.end())
		return iter->second;
	return "";
}

int64_t CHttpParam::GetParamInt(std::string strIndex)
{
	std::string strRet = GetParam(strIndex);
	return std::stoll(strRet);
}

void CHttpParam::Clear()
{
	m_vecParam.clear();
	m_mapParam.clear();
	m_strRet.clear();
	m_cType = 0;			//类型标识
	m_llKey = 0;			//key
	m_bPost = false;
	m_bRet = false;
	m_strFile.clear();
}

std::string CHttpParam::GetUrl()
{
	std::string str = m_strHost + m_strHead;
	if (m_bPost)
		return str;

	str += "?";
	str += GetFields();
	return str;
}

std::string CHttpParam::GetFields()
{
	std::string str;
	for (size_t i = 0; i < m_vecParam.size(); ++i)
	{
		str.append(m_vecParam[i]);
		if ((i + 1) < m_vecParam.size())
			str.append("&");
	}
	return str;
}