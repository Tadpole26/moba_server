#pragma once
#include "singleton.h"
#include "mongo_mgr.h"
#include "mysql_db.h"

class CDbInstance
{
public:
	CDbInstance();
	~CDbInstance();

	bool Init();
	void UnInit();
	void OnTime();

	bool CreateDBTable();

	bool ModifyRoleName(int64 llPlayerId, const std::string& strName);
	int InsertNewRole(int64 llPlayerId, const std::string& strName, bool bTransfer = false);
	void UpdateRoleInfo(int64 llPlayerId, const std::string& strName, uint32 uiLevel,
		uint32 uiVip, bool bLogin, int64 llLoginTime, int64 llLogoutTime);
	void UpdateRoleInfoMB(int64 llPlayerId, uint32 uiRmb, uint32 uiRecharge);
	bool GetUserLoginInfo(int64& llPlayerId, int& iProvince, std::string& strProvince
		, std::string& strSession, uint32& uiGmLevel, std::string& strDev
		, uint32& uiArea, std::string& strChannel, std::string& accName, std::string& strCityCode);
private:
	bool IsNameValid(int64 llPlayerId, const std::string& strName);

private:
	MysqlDb m_oMysql;
	MongoMgr m_oMongoMgr;

	std::string m_strTableName;
	NumTimer<30> m_oInternal;
};

#define SYS_DB_INS Singleton<CDbInstance>::getInstance()
