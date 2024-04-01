#include "CGameConstConfig.h"
#include "log_mgr.h"

bool CGameConstConfig::LoadConfig(const std::string& strContent)
{
	InitSheet("const_info", std::bind(&CGameConstConfig::AddConstInfo, this));

	return LoadCfgFromStr(strContent);
}

bool CGameConstConfig::AddConstInfo()
{
	std::string strName = GetDataChr("id");
	if (strName.compare("mail_count_max") == 0)
	{
		m_iMailCountMax = GetDataInt("value");
		return true;
	}
	Log_Warning("game const config not read %s", strName.c_str());
	return true;
}
