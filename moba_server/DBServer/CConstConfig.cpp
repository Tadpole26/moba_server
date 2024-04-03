#include "pch.h"
#include "CConstConfig.h"
#include "global_define.h"
#include "util_file.h"
#include "zookeeper.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "log_mgr.h"

bool CConstConfig::Init()
{
	InitSheet("common_config", std::bind(&CConstConfig::AddCommonCfg, this));
	InitSheet("db_config", std::bind(&CConstConfig::AddDBCfg, this));
	InitSheet("mongo_config", std::bind(&CConstConfig::AddMongoCfg, this));
	
	return LoadCfgFromXml(CONST_CONFIG);
}

bool CConstConfig::AddCommonCfg()
{
	m_uiGroupId = GetDataUInt("group");
	m_uiPlatId = GetDataUInt("platid");
	return true;
}

bool CConstConfig::AddDBCfg()
{
	m_strDBIp = GetDataChr("ip");
	m_uiDBPort = GetDataUInt("port");
	return true;
}

bool CConstConfig::AddMongoCfg()
{
	m_strMongoHost = GetDataChr("host");
	m_strMongoUser = GetDataChr("user");
	m_strMongoPasswd = GetDataChr("password");
	m_strMongoGameDb = GetDataChr("game_db");
	m_strMongoMailDb = GetDataChr("mail_db");
	m_strMongoAuth = GetDataChr("auth");
	return true;
}
