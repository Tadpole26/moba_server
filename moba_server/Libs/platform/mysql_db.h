#pragma once
#include <string>
#include "db_struct.h"
#include "global_define.h"
#include <mysql++.h>
#include <connection.h>

using namespace std;
using namespace mysqlpp;
class MysqlDb
{
public:
	static const int MAX_SQL_SIZE = 81920;
	MysqlDb();
	MysqlDb(const std::string& host, const std::string& user
		, const std::string& password, const std::string& name
		, int& port);
	~MysqlDb();
public:
	bool Init(const std::string& host, const std::string& user
		, const std::string& password, const std::string& name
		, const int& port);
	void SetDbName(const std::string& name) { m_strName = name; }

	bool DbConnect();
	void DbDisConnect() { m_pMysql.disconnect(); }
	bool Connected() { return m_pMysql.connected(); }

	bool DBSelect_One(sDBSelectReq& request);
	//查询,或者执行存储过程调用
	bool DBSelect(sDBSelectReq& request, fn_RetSqlCall& fnCall, size_t& retNum);
	bool DBSelect(const char* sql, fn_RetSqlCall& fnCall, size_t& retNum);
	//执行存储过程调用
	bool DBCallProc(const char* sql);

	//无返回值的操作
	int64 DBExec(const char* sql, eDBQueryType eType = eDB_None);
	int64 DBExec(sDBRequest& request);
	bool DBExec(std::vector<std::string>& vecCreateSql);
	int64 DBInsert(sDBRequest& request);
	int64 DBDelete(sDBRequest& request);
	int64 DBUpdate(sDBRequest& request);
	int64 DBIncValue(sDBRequest& request);
	int64 DBInsertUpdate(sDBRequest& request);
	bool DBPing();

	std::string GetHostName() { return m_strHost; }
	std::string GetUser() { return m_strUser; }
	std::string GetPassword() { return m_strPassword; }
	std::string GetName() { return m_strName; }
	uint32 GetPort() { return m_uiPort; }
	bool UpdateDBFile(const char* pszFileName, const char* pszDBName
		, int iDBId = 1, const char* pszTableName = NULL);
	bool UpdateLogFile(const char* pszFileName, const std::string& strDate);

private:
	Connection m_pMysql;
	char m_szQuery[MAX_SQL_SIZE];

	std::string m_strHost;
	std::string m_strUser;
	std::string m_strPassword;
	std::string m_strName;
	uint32 m_uiPort = 0;
	uint32 m_uiReconTimes = 0;

private:
	bool _DBSelect(const char* sql, fn_RetSqlCall& fnCall, sDBMap& ret
		, size_t& retNum, bool bOne);
	const char* GetDBLastError();
	const char* GetLastQuery();

	bool IsStrType(const mysqlpp::FieldTypes::value_type& stype);
	size_t fetchRows(StoreQueryResult& res, sDBMap& ret);
	std::string MakeInsert(sDBMap& mapData);
	std::string MakeInsert(sDBMap& mapCon, sDBMap& mapReq);
	std::string MakeUpdate(sDBMap& mapData);
	std::string MakeIncInsert(sDBMap& mapCon, sDBMap& mapReq);
	std::string MakeInc(sDBMap& mapData);
	std::string MakeFields(std::set<std::string>& setFields);
	std::string MakeCon(sDBMap& conditions);
	void MakeOpType(stringstream& ss, const std::string& skey
		, const std::string& sValue, eDBCalType eType = EDCT_EQ);
	void MakeOpType(stringstream& ss, const std::string& skey
		, const int64& sValue, eDBCalType eType = EDCT_EQ);

	bool OpenCreateDBFile(const std::string& strFileName, std::vector<std::string>& vecCreateSql
		, int iDBId, const std::string& pszTableName);
	bool OpenLogDBFile(const std::string& strFileName, std::vector<std::string>& vecCreateSql
		, const std::string& strDate);
	bool IsNetError(int iErr);
	bool StreamItem(bool& bFirst, std::stringstream& szName
		, std::stringstream& szValue, std::map<std::string, sDBMap::sDBItemNum>& mapData);
	bool StreamItem(bool& bFirst, std::stringstream& szName
		, std::stringstream& szValue, std::map<std::string, sDBMap::sDBItemStr>& mapData);
};
