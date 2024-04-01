#pragma once
#include <string>
#include "dtype.h"
#include "util_load_base.h"
//读取本地zk配置(ip port等等)
class CConstConfig : public CUtilLoadBase
{
public:
	bool Init();

	bool AddCommonCfg();
	bool AddCrossCfg();
	bool AddMongoCfg();
	bool AddMysqlCfg();
public:
	uint32									m_uiPlatId = 0;
	uint32									m_uiGroupId = 0;

	std::string								m_strCrossIp = "127.0.0.1";
	uint32 									m_uiCrossPort = 0;
	//std::string							m_strZkHost = "127.0.0.1:2181";
	//std::string							m_strServerConfigPath = "/game_config";
	//std::string 							m_strNormalConfigPath = "/normal_config";

	//mongodb 
	std::string					m_strMongoHost = "";
	std::string					m_strMongoUser = "";
	std::string					m_strMongoPasswd = "";
	std::string					m_strMongoGameName = "";
	std::string					m_strMongoAuth = "";

	//mysql 
	std::string					m_strMysqlHost = "";
	std::string					m_strMysqlUser = "";
	std::string					m_strMysqlPasswd = "";
	std::string					m_strMysqlAccName = "";
	uint32						m_uiMysqlPort = 0;
};
