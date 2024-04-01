#pragma once

#include <queue>
#include <mutex>
#include <thread>
#include <functional>

#include "db_struct.h"
#include "mongo_db.h"
#include "db_mgr.h"

class MongoMgr : public DBMgr
{
public:
	MongoMgr();
	~MongoMgr();

public:
	virtual bool Init(const std::string& hostname, const std::string& user
		, const std::string& pwd, const std::string& dbname, const std::string& auth
		, bool bAddArea = true);
	virtual void DoPing();
	virtual int64 ExecSQL(sDBRequest& oRequest);

private:
	MongoDb m_mongo;
};
