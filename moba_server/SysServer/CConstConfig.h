#pragma once
#include <string>
#include "dtype.h"
#include "util_load_base.h"

//Íø¹Ø·þÎñÆ÷ÅäÖÃ
class CConstConfig : public CUtilLoadBase
{
public:
	bool Init();

	bool AddCommonCfg();
	bool AddSysCfg();
	bool AddGameApiCfg();
	bool AddMongoCfg();
	bool AddMysqlCfg();
public:

	uint32_t					m_uiGroupId = 0;
	uint32_t					m_uiPlatId = 0;

	std::string					m_strApiAddr = "";

	std::string					m_strSysIp = "";
	uint32_t 					m_uiSysPort = 0;

	//mongodb 
	std::string					m_strMongoHost = "";
	std::string					m_strMongoUser = "";
	std::string					m_strMongoPasswd = "";
	std::string					m_strMongoAccName = "";
	std::string					m_strMongoAuth = "";

	//mysql 
	std::string					m_strMysqlHost = "";
	std::string					m_strMysqlUser = "";
	std::string					m_strMysqlPasswd = "";
	std::string					m_strMysqlAccName = "";
	uint32						m_uiMysqlPort = 0;
};
