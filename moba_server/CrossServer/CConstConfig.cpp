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
	InitSheet("cross_config", std::bind(&CConstConfig::AddCrossCfg, this));
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

bool CConstConfig::AddCrossCfg()
{
	m_strCrossIp = GetDataChr("ip");
	m_uiCrossPort = GetDataUInt("port");
	//m_strZkHost = GetDataChr("zk_host");;
	//m_strServerConfigPath = GetDataChr("server_node");
	//m_strNormalConfigPath = GetDataChr("normal_node");
	return true;
}

bool CConstConfig::AddMongoCfg()
{
	m_strMongoHost = GetDataChr("host");
	m_strMongoUser = GetDataChr("user");
	m_strMongoPasswd = GetDataChr("passowrd");
	m_strMongoGameName = GetDataChr("game_name");
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