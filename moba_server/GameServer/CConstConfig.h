#pragma once
#include <string>
#include "dtype.h"
#include "util_load_base.h"
//��ȡ����zk����(ip port�ȵ�)
class CConstConfig : public CUtilLoadBase
{
public:
	bool Init();

	bool AddCommonCfg();
	bool AddGameCfg();
	bool AddSysCfg();
	bool AddCrossCfg();
	bool AddDBCfg();
	bool AddMongoCfg();
	bool AddMysqlCfg();
public:
	uint32									m_uiPlatId = 0;
	uint32									m_uiGroupId = 0;

	std::string								m_strGameIp = "127.0.0.1";
	uint32 									m_uiGamePort = 0;
	std::string								m_strZkHost = "127.0.0.1:2181";
	std::string								m_strServerConfigPath = "/game_config";
	std::string 							m_strNormalConfigPath = "/normal_config";
	int64_t									m_llOpenTime = 0;							//����ʱ��
	int										m_iReloginTime = 120;						//�صǳ�ʱʱ��

	//sys server
	std::string					m_strSysIp = "";
	uint32						m_uiSysPort = 0;

	//cross server
	std::string					m_strCrossIp = "";
	uint32						m_uiCrossPort = 0;

	//DB server
	std::string					m_strDBIp = "";
	uint32						m_uiDBPort = 0;

	//mongodb 
	std::string					m_strMongoHost = "";
	std::string					m_strMongoUser = "";
	std::string					m_strMongoPasswd = "";
	std::string					m_strMongoGameDb = "";
	std::string					m_strMongoMailDb = "";
	std::string					m_strMongoAuth = "";

	//mysql 
	std::string					m_strMysqlHost = "";
	std::string					m_strMysqlUser = "";
	std::string					m_strMysqlPasswd = "";
	std::string					m_strMysqlAccName = "";
	uint32						m_uiMysqlPort = 0;
};
