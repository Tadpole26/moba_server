#pragma once
#include <string>
#include <vector>
#include "dtype.h"
#include <map>

class CHttpParam
{
public:
	CHttpParam();
	virtual ~CHttpParam();

	void AddParam(std::string strIndex, std::string strValue);
	void AddParam(std::string strIndex, int64_t llValue);

	std::string GetParam(std::string strIndex);
	int64_t GetParamInt(std::string strIndex);

	void Clear();
	std::string GetUrl();
	std::string GetFields();
	std::map<std::string, std::string>& GetMapFields() { return m_mapParam; }

public:
	uint8_t m_cType = 0;				//类型标识
	int64_t m_llKey = 0;				//key
	bool m_bPost = false;
	bool m_bRet = false;

	std::string m_strHost;
	std::string m_strHead;
	std::string m_strRet;
	std::string m_strFile;
	int32_t m_iCode = 0;
	std::vector<std::string> m_vecParam;
	std::map<std::string, std::string> m_mapParam;
};