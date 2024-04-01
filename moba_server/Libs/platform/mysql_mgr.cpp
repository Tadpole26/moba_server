#include "pch.h"
#include "mysql_mgr.h"

MysqlMgr::MysqlMgr()
{
}

MysqlMgr::~MysqlMgr()
{
}

bool MysqlMgr::Init(const std::string& strHost, const std::string& strUser
	, const std::string& strPassword, const std::string& strName, const int& iPort)
{
	if (!m_mysql.Init(strHost, strUser, strPassword, strName, iPort))
		return false;

	return DBMgr::Init();
}

void MysqlMgr::DoPing()
{
	m_mysql.DBPing();
}

void MysqlMgr::PushSQL(const char* strSql)
{
	sDBRequest item;
	item.Init(eDB_String, strSql);
	DBMgr::PushData(item);
}

int64 MysqlMgr::ExecSQL(sDBRequest& oRequest)
{
	if (oRequest.type() != eDB_String)
		return m_mysql.DBExec(oRequest);
	else
		return m_mysql.DBExec(oRequest.tbl().c_str());
}