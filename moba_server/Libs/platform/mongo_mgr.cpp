#include "pch.h"
#include "mongo_mgr.h"

MongoMgr::MongoMgr()
{
}

MongoMgr::~MongoMgr()
{
}

bool MongoMgr::Init(const std::string& hostname, const std::string& user
	, const std::string& pwd, const std::string& dbname, const std::string& auth
	, bool bAddArea)
{
	if (!m_mongo.Init(hostname, user, pwd, dbname, auth, bAddArea))
		return false;
	return DBMgr::Init();
}

void MongoMgr::DoPing()
{
	m_mongo.DbPing();
}

int64 MongoMgr::ExecSQL(sDBRequest& oRequest)
{
	if (oRequest.type() != eDB_None)
		return m_mongo.DBExec(oRequest) ? true : -1;

	Log_Error("sql tbl:%s, sectable:%s type none !!!", oRequest.tbl().c_str()
		, oRequest.sectbl().name().c_str());
	return -1;
}

