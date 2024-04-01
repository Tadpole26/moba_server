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
	bool AddDBCfg();
	bool AddMongoCfg();
public:
	uint32									m_uiPlatId = 0;
	uint32									m_uiGroupId = 0;

	std::string								m_strDBIp = "127.0.0.1";
	uint32 									m_uiDBPort = 0;
	//mongodb 
	std::string					m_strMongoHost = "";
	std::string					m_strMongoUser = "";
	std::string					m_strMongoPasswd = "";
	std::string					m_strMongoGameDb = "";
	std::string					m_strMongoMailDb = "";
	std::string					m_strMongoAuth = "";

};
