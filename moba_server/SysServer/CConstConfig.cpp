#include "CConstConfig.h"
#include <string>
#include "global_define.h"
#include "util_file.h"
#include "log_mgr.h"

bool CConstConfig::Init()
{
	InitSheet("common_config", std::bind(&CConstConfig::AddCommonCfg, this));
	InitSheet("sys_config", std::bind(&CConstConfig::AddSysCfg, this));
	InitSheet("game_api", std::bind(&CConstConfig::AddGameApiCfg, this));
	InitSheet("mongo_config", std::bind(&CConstConfig::AddMongoCfg, this));
	InitSheet("mysql_config", std::bind(&CConstConfig::AddMysqlCfg, this));

	return LoadCfgFromXml(CONST_CONFIG);
}

bool CConstConfig::AddCommonCfg()
{
	m_uiGroupId = GetDataUInt("group");
	m_uiPlatId = GetDataUInt("platid");
	return true;
}

bool CConstConfig::AddSysCfg()
{
	m_strSysIp = GetDataChr("ip");
	m_uiSysPort = GetDataUInt("port");
	return true;
}

bool CConstConfig::AddGameApiCfg()
{
	m_strApiAddr = GetDataChr("address");
	return true;
}

bool CConstConfig::AddMongoCfg()
{
	m_strMongoHost = GetDataChr("host");
	m_strMongoUser = GetDataChr("user");
	m_strMongoPasswd = GetDataChr("passowrd");
	m_strMongoAccName = GetDataChr("game_name");
	m_strMongoAuth = GetDataChr("auth");
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