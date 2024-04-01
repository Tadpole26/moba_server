#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include "util_spthread.h"
#include "singleton.h"
#include "db_struct.h"
#include "mysql_db.h"
#include "db_mgr.h"

class MysqlMgr : public DBMgr
{
public:
	MysqlMgr();
	~MysqlMgr();
public:
	virtual bool Init(const std::string& strHost, const std::string& strUser
		, const std::string& strPassword, const std::string& strName, const int& iPort);

	virtual void PushSQL(const char* strSql);

	virtual void DoPing();
	virtual int64 ExecSQL(sDBRequest& oRequest);
private:
	MysqlDb m_mysql;
};