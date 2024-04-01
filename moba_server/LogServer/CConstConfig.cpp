#include "CConstConfig.h"
#include <string>
#include "global_define.h"
#include "util_file.h"
#include "log_mgr.h"

bool CConstConfig::Init()
{
	InitSheet("common_config", std::bind(&CConstConfig::AddCommonCfg, this));
	InitSheet("log_config", std::bind(&CConstConfig::AddLogCfg, this));
	InitSheet("mysql_config", std::bind(&CConstConfig::AddMysqlCfg, this));

	return LoadCfgFromXml(CONST_CONFIG);
}

bool CConstConfig::AddCommonCfg()
{
	m_uiGroupId = GetDataUInt("group");
	m_uiPlatId = GetDataUInt("platid");
	return true;
}

bool CConstConfig::AddLogCfg()
{
	m_strLogIp = GetDataChr("ip");
	m_uiLogPort = GetDataUInt("port");
	return true;
}

bool CConstConfig::AddMysqlCfg()
{
	m_strMysqlHost = GetDataChr("host");
	m_strMysqlUser = GetDataChr("user");
	m_strMysqlPasswd = GetDataChr("password");
	m_strMysqlAccName = GetDataChr("game_name");
	m_uiMysqlPort = GetDataUInt("port");
	return true;
}