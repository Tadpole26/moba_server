#include "pch.h"
#include "util_load_base.h"
#include "log_mgr.h"
#include "util_file.h"
#include <tinyxml.h>
#include "util_string.h"

void CUtilLoadBase::InitSheet(const char* pSheet, fn_read fn, bool bCanEmpty)
{
	auto iter = m_mapSheet.find(pSheet);
	if (iter != m_mapSheet.end())
	{
		iter->second.pFnRead = fn;
		iter->second.bCanEmpty = bCanEmpty;
		iter->second.bInit = false;
		iter->second.uiLines = 0;
	}
	else
	{
		sheet stItem;
		stItem.pFnRead = fn;
		stItem.bCanEmpty = bCanEmpty;
		stItem.bInit = false;
		m_mapSheet.insert(std::make_pair(pSheet, stItem));
	}
}

bool CUtilLoadBase::IsValid()
{
	bool bRet = true;
	for (const auto& sheet : m_mapSheet)
	{
		if (!sheet.second.bCanEmpty && (!sheet.second.bInit) && sheet.second.uiLines > 0)
		{
			Log_Error("%s sheet:%s", m_strFile.c_str(), sheet.first.c_str());
			bRet = false;
		}
	}
	return bRet;
}

bool CUtilLoadBase::ReadString(const char* pField, std::string& strRet)
{
	auto iter = m_mapRowData.find(pField);
	if (iter == m_mapRowData.end())
		return false;
	strRet = iter->second;
	return true;
}

std::string CUtilLoadBase::GetDataChr(const char* pField, const std::string& strDef)
{
	assert(pField);
	std::string strRet;
	if (!ReadString(pField, strRet))
		return strDef;
	return strRet;
}

int32_t CUtilLoadBase::GetDataInt(const char* pField, const int32_t iDef)
{
	assert(pField);
	std::string strRet;
	if (!ReadString(pField, strRet))
		return iDef;
	int32_t iRet = std::stoi(strRet);
	if (iRet < 0)
	{
		Log_Warning("%s-%s: %s:%d load!", m_strFile.c_str(), m_strCurSheet.c_str(),
			pField, iDef)
	}
	return iRet;
}

bool CUtilLoadBase::GetDataBool(const char* pField, const bool bDef)
{
	assert(pField);
	std::string strRet;
	if (!ReadString(pField, strRet))
		return bDef;
	return std::stoi(strRet) == 1;
}

uint32_t CUtilLoadBase::GetDataUInt(const char* pField, const uint32_t uiDef)
{
	assert(pField);
	std::string strRet;
	if (!ReadString(pField, strRet))
		return uiDef;
	uint32_t iRet = std::stoi(strRet);
	if (iRet < 0)
	{
		Log_Error("%s-%s: %s:%d load!", m_strFile.c_str(), m_strCurSheet.c_str(), pField, iRet);
		return uiDef;
	}
	return (uint32_t)(iRet);
}

double CUtilLoadBase::GetDataDouble(const char* pField, const double dDef)
{
	assert(pField);
	std::string strRet;
	if (!ReadString(pField, strRet))
		return dDef;
	return std::stod(strRet);
}

time_t CUtilLoadBase::GetDataTime(const char* pField, const time_t tDef)
{
	std::string strText;
	auto iter = m_mapRowData.find(pField);
	if (iter == m_mapRowData.end())
		return tDef;

	if (strText.empty())
		return tDef;
	else
	{
		time_t tRet = 0;
		if (DateToTime(tRet, strText))
			return tRet;
		else
		{
			Log_Error("%s-%s: %s load value err:%s !", m_strFile.c_str(),
				m_strCurSheet.c_str(), pField, strText.c_str());
		}
	}
	return tDef;
}

int64_t CUtilLoadBase::GetDataInt64(const char* pField, const int64_t llDef)
{
	assert(pField);
	std::string strRet;
	if (!ReadString(pField, strRet))
		return llDef;
	int64_t llRet = std::stoll(strRet);
	if (llRet < 0)
		Log_Warning("%s-%s: %s:%lld load!", m_strFile.c_str(), m_strCurSheet.c_str(),
			pField, llRet);
	return llRet;
}

bool CUtilLoadBase::LoadCfgFromXml(const char* pXmlFileName)
{
	if (nullptr != pXmlFileName)
		m_strFile = pXmlFileName;

	if (m_strFile.empty())
		return false;

	std::string strPath = get_local_path(pXmlFileName);
	std::unique_ptr<TiXmlDocument> pDoc(new TiXmlDocument(strPath.c_str()));
	if (!pDoc)
	{
		Log_Error("new %s TiXmlDocument fail!", pXmlFileName);
		return false;
	}

	if (!pDoc->LoadFile())
	{
		Log_Error("%s load %s fail!", typeid(*this).name(), pXmlFileName);
		return false;
	}

	TiXmlElement* pRoot = pDoc->RootElement();
	if (!pRoot)
	{
		Log_Error("%s:%s root node is empty!", typeid(*this).name(), pXmlFileName);
		return false;
	}

	TiXmlElement* pElem = pRoot->FirstChildElement();
	while (pElem)
	{
		m_mapRowData.clear();
		m_strCurSheet = pElem->Value();
		auto iterFunc = m_mapSheet.find(pElem->Value());
		if (iterFunc == m_mapSheet.end() || iterFunc->second.pFnRead == nullptr)
		{
			pElem = pElem->NextSiblingElement();
			continue;
		}

		TiXmlElement* pElemNode = pElem->FirstChildElement();
		while (pElemNode)
		{
			std::string strKey = pElemNode->Value();
			const char* pText = pElemNode->GetText();
			std::string strValue = (pText == nullptr ? "" : pText);

			strKey = trim_all(strKey);
			strValue = trim_s(strValue);

			if (strKey.empty())
			{
				TiXmlPrinter printer;
				printer.SetStreamPrinting();
				pElem->Accept(&printer);
				Log_Error("%s:%s root node is empty!, data: %s", 
					typeid(*this).name(), pXmlFileName, printer.CStr());
				return false;
			}
			m_mapRowData.insert(std::make_pair(std::move(strKey), std::move(strValue)));
			pElemNode = pElemNode->NextSiblingElement();
		}

		iterFunc->second.uiLines++;
		if (!(iterFunc->second.pFnRead)())
		{
			TiXmlPrinter printer;
			printer.SetStreamPrinting();
			pElem->Accept(&printer);
			Log_Error("%s: %s load %s error!, data: %s",
				typeid(*this).name(), m_strFile.c_str(), pElem->Value(), printer.CStr());
			return false;
		}
		iterFunc->second.bInit = true;
		pElem = pElem->NextSiblingElement();
	}
	return IsValid();
}

bool CUtilLoadBase::LoadCfgFromStr(const std::string& strCfg)
{
	if (strCfg.empty())
		return false;

	std::unique_ptr<TiXmlDocument> pDoc(new TiXmlDocument());
	if (!pDoc)
	{
		Log_Error("new %s TiXmlDocument fail!", strCfg.c_str());
		return false;
	}

	if (!pDoc->Parse(strCfg.c_str()))
	{
		Log_Error("%s load %s fail!", typeid(*this).name(), strCfg.c_str());
		return false;
	}

	TiXmlElement* pRoot = pDoc->RootElement();
	if (!pRoot)
	{
		Log_Error("%s:%s root node is empty!", typeid(*this).name(), strCfg.c_str());
		return false;
	}

	TiXmlElement* pElem = pRoot->FirstChildElement();
	while (pElem)
	{
		m_mapRowData.clear();
		m_strCurSheet = pElem->Value();
		auto iterFunc = m_mapSheet.find(pElem->Value());
		if (iterFunc == m_mapSheet.end() || iterFunc->second.pFnRead == nullptr)
		{
			pElem = pElem->NextSiblingElement();
			continue;
		}

		TiXmlElement* pElemNode = pElem->FirstChildElement();
		while (pElemNode)
		{
			std::string strKey = pElemNode->Value();
			const char* pText = pElemNode->GetText();
			std::string strValue = (pText == nullptr ? "" : pText);

			strKey = trim_all(strKey);
			strValue = trim_s(strValue);

			if (strKey.empty())
			{
				TiXmlPrinter printer;
				printer.SetStreamPrinting();
				pElem->Accept(&printer);
				Log_Error("%s:%s root node is empty!, data: %s",
					typeid(*this).name(), strCfg.c_str(), printer.CStr());
				return false;
			}
			m_mapRowData.insert(std::make_pair(std::move(strKey), std::move(strValue)));
			pElemNode = pElemNode->NextSiblingElement();
		}

		iterFunc->second.uiLines++;
		if ((iterFunc->second.pFnRead)() == false)
		{
			TiXmlPrinter printer;
			printer.SetStreamPrinting();
			pElem->Accept(&printer);
			Log_Error("%s: %s load %s error!, data: %s",
				typeid(*this).name(), strCfg.c_str(), pElem->Value(), printer.CStr());
			return false;
		}
		iterFunc->second.bInit = true;
		pElem = pElem->NextSiblingElement();
	}
	return IsValid();
}

bool CUtilLoadBase::LoadConfig(const std::string& strContent)
{
	return true;
}