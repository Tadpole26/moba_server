#include "pch.h"
#include "mysql_db.h"
#include "log_mgr.h"
//#include "mysql.h"
#include "util_file.h"
#include "util_strop.h"

MysqlDb::MysqlDb()
{
	m_strHost = "localhost";
	m_strUser = "root";
	m_strPassword = "";
	m_uiPort = 3306;
	m_strName = "xproject";
	memset(m_szQuery, 0, sizeof(m_szQuery));
}

MysqlDb::MysqlDb(const std::string& host, const std::string& user
	, const std::string& password, const std::string& name
	, int& port)
{
	m_strHost = host;
	m_strUser = user;
	m_strPassword = password;
	m_uiPort = port;
	m_strName = name;
	memset(m_szQuery, 0, sizeof(m_szQuery));
}

MysqlDb::~MysqlDb()
{
}

bool MysqlDb::Init(const std::string& host, const std::string& user
	, const std::string& password, const std::string& name
	, const int& port)
{
	m_strHost = host;
	m_strUser = user;
	m_strPassword = password;
	m_uiPort = port;
	m_strName = name;
	memset(m_szQuery, 0, sizeof(m_szQuery));

	return DbConnect();
}

bool MysqlDb::DbConnect()
{
	time_t llBegin = std::time(nullptr);
	try
	{
		m_pMysql.disable_exceptions();
		m_pMysql.disconnect();
		m_pMysql.set_option(new SetCharsetNameOption("utf8"));
		m_pMysql.set_option(new ConnectTimeoutOption(30));

		if (m_pMysql.connect(m_strName.c_str(), m_strHost.c_str()
			, m_strUser.c_str(), m_strPassword.c_str(), m_uiPort))
		{
			m_pMysql.query("SET NAMES utf8");
			Log_Info("DB connection success!, second:%lld"
				, std::time(nullptr) - llBegin);
			return true;
		}
		m_pMysql.enable_exceptions();
	}
	catch (Exception& e)
	{
		Log_Error("failed!, error:%s, err:%s, second:%lld"
			, GetDBLastError(), e.what(), std::time(nullptr) - llBegin)
	}
	Log_Error("DB connection failed:%s, second:%lld"
		, GetDBLastError(), std::time(nullptr) - llBegin);
	return false;
}

bool MysqlDb::DBSelect_One(sDBSelectReq& request)
{
	request.clear_ret();
	try
	{
		std::string strFields = MakeFields(request.Fields());
		std::string strCon = MakeCon(request.Con());

		sprintf_safe(m_szQuery, sizeof(m_szQuery), "SELECT %s FROM `%s` WHERE %s;"
			, strFields.c_str(), request.tbl().c_str(), strCon.c_str());
		fn_RetSqlCall fnCall = [&](sDBMap& ret)->bool { return true; };
	}
	catch (const std::exception& err)
	{
		Log_Error("table:%s, type:%d, err:%s", request.tbl().c_str(), request.type(), err.what());
	}
	return true;
}

bool MysqlDb::DBSelect(sDBSelectReq& request, fn_RetSqlCall& fnCall, size_t& retNum)
{
	request.clear_ret();
	try
	{
		std::string strFields = MakeFields(request.Fields());
		std::string strCon = MakeCon(request.Con());

		sprintf_safe(m_szQuery, sizeof(m_szQuery), "SELECT %s FROM `%s` WHERE %s;"
			, strFields.c_str(), request.tbl().c_str(), strCon.c_str());
		return _DBSelect(m_szQuery, fnCall, request.Ret_Main(), retNum, false);
	}
	catch (const std::exception& err)
	{
		Log_Error("table:%s, type:%d, err:%s", request.tbl().c_str()
			, request.type(), err.what());
	}
	return true;
}

bool MysqlDb::DBSelect(const char* sql, fn_RetSqlCall& fnCall, size_t& retNum)
{
	sDBMap ret;
	return _DBSelect(sql, fnCall, ret, retNum, false);
}

bool MysqlDb::_DBSelect(const char* sql, fn_RetSqlCall& fnCall, sDBMap& ret
	, size_t& retNum, bool bOne)
{
	StoreQueryResult res;
	size_t retList = 0;
	for (size_t i = 0; i < 60; ++i)
	{
		try
		{
			Query query = m_pMysql.query(sql);
			if (!(res = query.store()))
			{
				int iErrNum = m_pMysql.errnum();
				if (iErrNum == 0 || iErrNum == 1312)	//没有返回集
					return false;

				Log_Error("SQL:%s  error:%s, errorno:%d, reconnect ts:%d",
					sql, GetDBLastError(), m_pMysql.errnum(), i);
				if (IsNetError(query.errnum()))
				{
					if (!DBPing())
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				}
				else
					return false;
			}
			else
			{
				++retList;
				retNum = fetchRows(res, ret);
				if (fnCall != nullptr) fnCall(ret);
				while (res = query.store_next())
				{
					if (!bOne)
					{
						++retList;
						fetchRows(res, ret);
						if (fnCall != nullptr) fnCall(ret);
					}
				}
				if (retList > 1) retNum = retList;
				return true;
			}
		}
		catch (Exception& e)
		{
			Log_Error("Excute SQL:%s failed! error:%s, err:%s, errono:%d, reconnect ts:%d"
				, sql, GetDBLastError(), e.what(), m_pMysql.errnum(), i);
			if (!DBPing())
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
	return false;
}

bool MysqlDb::DBCallProc(const char* sql)
{
	fn_RetSqlCall fnCall = [&](sDBMap& ret)->bool { return true; };
	size_t iRetNum = 0;
	sDBMap retMap;
	return _DBSelect(sql, fnCall, retMap, iRetNum, true);
}

int64 MysqlDb::DBExec(const char* sql, eDBQueryType eType)
{
	assert(sql);
	for (size_t i = 0; i < 60; ++i)
	{
		int64 llNumRows = 0;
		try
		{
			SimpleResult res;
			Query query = m_pMysql.query();
			query << sql;

			res = query.execute();
			if (!res)
			{
				int iErrNum = m_pMysql.errnum();
				if (iErrNum == 0 || iErrNum == 1312)	//没有返回集
					return 0;

				llNumRows = -1;
				Log_Error("Excute SQL:%s failed! error:%s, errono:%d, reconnect ts:%s"
					, sql, query.error(), query.errnum(), i);

				if (IsNetError(query.errnum()))
				{
					if (!DBPing())
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				}
				else
					return llNumRows;
			}
			else
			{
				llNumRows = (int64)res.rows();
				return llNumRows;
			}
		}
		catch (Exception& e)
		{
			Log_Error("Excete SQL:%s failed! error:%s, err:%s, errono:%d, reconnect ts:%d"
				, sql, GetDBLastError(), e.what(), m_pMysql.errnum(), i);
			llNumRows = -1;

			if (!DBPing())
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
	return -1;
}

int64 MysqlDb::DBExec(sDBRequest& request)
{
	switch (request.type())
	{
	case eDB_Update:
		return DBUpdate(request);
		break;
	case eDB_Delete:
		return DBDelete(request);
		break;
	case eDB_InsertUpdate:
		return DBInsertUpdate(request);
		break;
	case eDB_Insert:
		return DBInsert(request);
		break;
	case eDB_IncValue:
		return DBIncValue(request);
		break;
	default:
		Log_Error("other op type: %d", request.type());
		break;
	}
	return 0;
}


bool MysqlDb::DBExec(std::vector<std::string>& vecCreateSql)
{
	for (size_t i = 0; i < vecCreateSql.size(); i++)
	{
		std::string strSql = vecCreateSql[i];
		strSql = trim_s(strSql);
		if (strSql.empty())
		{
			Log_Warning("find empty sql statement");
			continue;
		}
		DBExec(strSql.c_str(), eDB_None);
	}
	return true;
}

size_t MysqlDb::fetchRows(StoreQueryResult& res, sDBMap& ret)
{
	std::string strFieldName, strFieldValue;
	ret.clear();
	std::vector<bool> bType;
	bType.resize(res.num_fields(), false);
	for (size_t i = 0; i < res.num_rows(); ++i)
	{
		for (size_t r = 0; r < res.num_fields(); ++r)
		{
			if (i == 0)
			{
				if (IsStrType(res.field_type((int)r)))
					bType[r] = true;
			}
			strFieldName = std::string(res.field_name((int)r));
			size_t uiLen = res[i][(int)r].length();
			strFieldValue = std::string(res[i][(int)r].c_str(), uiLen);
			if (bType[r])
				ret.AddStr(strFieldName, strFieldValue);
			else
				ret.Add(strFieldName, std::stoll(strFieldValue));
		}
	}
	return res.num_rows();
}

const char* MysqlDb::GetDBLastError()
{
	return m_pMysql.error();
}

const char* MysqlDb::GetLastQuery()
{
	return m_szQuery;
}

bool MysqlDb::IsStrType(const mysqlpp::FieldTypes::value_type& stype)
{
	if (stype == typeid(std::string))
		return true;
	else if (stype == typeid(mysqlpp::String))
		return true;
	else if (stype == typeid(mysqlpp::sql_date))
		return true;
	else if (stype == typeid(mysqlpp::sql_time))
		return true;
	else if (stype == typeid(mysqlpp::sql_datetime))
		return true;
	else if (stype == typeid(mysqlpp::sql_set))
		return true;

	return false;
}

int64 MysqlDb::DBInsert(sDBRequest& request)
{
	try
	{
		std::string strField = MakeInsert(request.reqMap());
		sprintf_safe(m_szQuery, sizeof(m_szQuery), "INSERT INTO `%s` %s;"
			, request.tbl().c_str(), strField.c_str());

		return DBExec(m_szQuery, eDB_Insert);
	}
	catch (const std::exception& err)
	{
		Log_Error("table:%s, type:%d, err:%s", request.tbl().c_str()
			, request.type(), err.what());
	}
	return -1;
}

int64 MysqlDb::DBDelete(sDBRequest& request)
{
	try
	{
		std::string strCondition = MakeCon(request.Con());
		sprintf_safe(m_szQuery, sizeof(m_szQuery), "DELETE FROM `%s` WHERE %s;"
			, request.tbl().c_str(), strCondition.c_str());

		return DBExec(m_szQuery, eDB_Delete);
	}
	catch (const std::exception& err)
	{
		Log_Error("table:%s, tyep:%d, err:%s", request.tbl().c_str()
			, request.type(),  err.what());
	}
	return -1;
}

int64 MysqlDb::DBUpdate(sDBRequest& request)
{
	try
	{
		std::string strField = MakeUpdate(request.reqMap());
		std::string strCondition = MakeCon(request.Con());
		sprintf_safe(m_szQuery, sizeof(m_szQuery), "UPDATE `%s` SET %s WHERE %s;"
			, request.tbl().c_str(), strField.c_str(), strCondition.c_str());
		return DBExec(m_szQuery, eDB_Update);
	}
	catch (const std::exception& err)
	{
		Log_Error("table:%s, type:%d, err:%s", request.tbl().c_str()
			, request.type(), err.what());
	}
	return -1;
}

int64 MysqlDb::DBIncValue(sDBRequest& request)
{
	try
	{
		std::string strInsert = MakeIncInsert(request.Con(), request.reqMap());
		std::string strUpdate = MakeInc(request.reqMap());
		sprintf_safe(m_szQuery, sizeof(m_szQuery), "INSERT INTO `%s` %s on duplicate key update %s;"
			, request.tbl().c_str(), strInsert.c_str(), strUpdate.c_str());
		return DBExec(m_szQuery, eDB_IncValue);
	}
	catch (const std::exception& err)
	{
		Log_Error("table:%s, type:%d, err:%s", request.tbl().c_str()
			, request.type(), err.what());
	}
	return -1;
}

int64 MysqlDb::DBInsertUpdate(sDBRequest& request)
{
	try
	{
		std::string strInsert = MakeInsert(request.Con(), request.reqMap());
		std::string strUpdate = MakeUpdate(request.reqMap());
		sprintf_safe(m_szQuery, sizeof(m_szQuery), "INSERT INTO `%s` %s on duplicate key update %s;"
			, request.tbl().c_str(), strInsert.c_str(), strUpdate.c_str());
		return DBExec(m_szQuery, eDB_Insert);
	}
	catch (const std::exception& err)
	{
		Log_Error("table:%s, type:%d, err:%s", request.tbl().c_str()
			, request.type(), err.what());
	}
	return -1;
}

bool MysqlDb::StreamItem(bool& bFirst, std::stringstream& szName
	, std::stringstream& szValue, std::map<std::string, sDBMap::sDBItemNum>& mapData)
{
	for (auto iter = mapData.begin(); iter != mapData.end(); ++iter)
	{
		if (!bFirst)
		{
			szName << ",";
			szValue << ",";
		}
		bFirst = false;
		szName << iter->first;
		szValue << iter->second.value;
	}
	return bFirst;
}


bool MysqlDb::StreamItem(bool& bFirst, std::stringstream& szName
	, std::stringstream& szValue, std::map<std::string, sDBMap::sDBItemStr>& mapData)
{
	for (auto iter = mapData.begin(); iter != mapData.end(); ++iter)
	{
		if (!bFirst)
		{
			szName << ",";
			szValue << ",";
		}
		bFirst = false;

		char szTemp[1024 * 8] = { 0 };
		mysql_escape_string(szTemp, iter->second.value.c_str(), (unsigned long)iter->second.value.length());
		szTemp[sizeof(szTemp) - 1] = '\0';

		szName << iter->first;
		szValue << "'" << szTemp << "'";
	}
	return bFirst;
}

std::string MysqlDb::MakeInsert(sDBMap& mapData)
{
	std::stringstream szName;
	std::stringstream szValue;
	bool bFirst = true;
	bFirst = StreamItem(bFirst, szName, szValue, mapData.MapNum());
	bFirst = StreamItem(bFirst, szName, szValue, mapData.MapStr());

	sprintf_safe(m_szQuery, sizeof(m_szQuery), "(%s)VALUES(%s)"
		, szName.str().c_str(), szValue.str().c_str());

	return m_szQuery;
}

std::string MysqlDb::MakeInsert(sDBMap& mapCon, sDBMap& mapReq)
{
	std::stringstream szName;
	std::stringstream szValue;
	bool bFirst = true;
	
	bFirst = StreamItem(bFirst, szName, szValue, mapCon.MapNum());
	bFirst = StreamItem(bFirst, szName, szValue, mapCon.MapStr());
	bFirst = StreamItem(bFirst, szName, szValue, mapReq.MapNum());
	bFirst = StreamItem(bFirst, szName, szValue, mapReq.MapStr());

	sprintf_safe(m_szQuery, sizeof(m_szQuery), "(%s)VALUES(%s)"
		, szName.str().c_str(), szValue.str().c_str());

	return m_szQuery;
}

std::string MysqlDb::MakeUpdate(sDBMap& mapData)
{
	std::stringstream szValue;
	bool bFirst = true;

	for (auto iter = mapData.MapNum().begin(); iter != mapData.MapNum().end(); ++iter)
	{
		if (!bFirst) szValue << ",";

		bFirst = false;
		MakeOpType(szValue, iter->first, iter->second.value);
	}

	for (auto iter = mapData.MapStr().begin(); iter != mapData.MapStr().end(); ++iter)
	{
		if (!bFirst) szValue << ",";
		
		bFirst = false;
		char szTemp[1024 * 4] = { 0 };
		mysql_escape_string(szTemp, iter->second.value.c_str(), (unsigned long)iter->second.value.length());
		szTemp[sizeof(szTemp) - 1] = '\0';

		MakeOpType(szValue, iter->first, szTemp);
	}
	return szValue.str();
}

std::string MysqlDb::MakeIncInsert(sDBMap& mapCon, sDBMap& mapReq)
{
	std::stringstream szName;
	std::stringstream szValue;
	bool bFirst = true;

	bFirst = StreamItem(bFirst, szName, szValue, mapCon.MapNum());
	bFirst = StreamItem(bFirst, szName, szValue, mapCon.MapStr());
	bFirst = StreamItem(bFirst, szName, szValue, mapReq.MapNum());

	sprintf_safe(m_szQuery,sizeof(m_szQuery), "(%s)VALUES(%s)"
		, szName.str().c_str(), szValue.str().c_str());

	return m_szQuery;
}

std::string MysqlDb::MakeInc(sDBMap& mapData)
{
	std::stringstream szValue;
	bool bFirst = true;
	for (auto iter = mapData.MapNum().begin(); iter != mapData.MapNum().end(); ++iter)
	{
		if (!bFirst) szValue << ',';

		bFirst = false;
		szValue << iter->first << "=" << iter->first << "+'" << iter->second.value << "'";
	}
	return szValue.str();
}

std::string MysqlDb::MakeFields(std::set<std::string>& setFields)
{
	stringstream ss;
	bool bFirst = true;
	for (auto& item : setFields)
	{
		if (bFirst) ss << ',';
		bFirst = false;
		ss << item;
	}
	return ss.str();
}

std::string MysqlDb::MakeCon(sDBMap& conditions)
{
	std::stringstream szValue;
	bool bFirst = true;

	for (auto iter = conditions.MapNum().begin(); iter != conditions.MapNum().end(); ++iter)
	{
		if (bFirst) szValue << " AND ";
		szValue << "(";
		MakeOpType(szValue, iter->first, iter->second.value, iter->second.caltype);
		szValue << ")";
		bFirst = false;
	}

	for (auto iter = conditions.MapStr().begin(); iter != conditions.MapStr().end(); ++iter)
	{
		if (!bFirst) szValue << " AND ";
		
		char szTemp[1024 * 8] = { 0 };
		mysql_escape_string(szTemp, iter->second.value.c_str(), (unsigned long)iter->second.value.length());
		szTemp[sizeof(szTemp) - 1] = '\0';

		szValue << "(";
		MakeOpType(szValue, iter->first, szTemp, iter->second.caltype);
		szValue << ")";
		bFirst = false;
	}
	return szValue.str();
}

bool MysqlDb::DBPing()
{
	if (!m_pMysql.ping())
	{
		++m_uiReconTimes;
		Log_Error("failed! recon ts:%u", m_uiReconTimes);
		if (DbConnect())
		{
			m_uiReconTimes = 0;
			return true;
		}
		else
			return false;
	}
	return true;
}

void MysqlDb::MakeOpType(stringstream& ss, const std::string& skey
	, const std::string& sValue, eDBCalType eType)
{
	switch (eType)
	{
	case EDCT_EQ:
		ss << skey << "='" << sValue << "'";
		break;
	case EDCT_GT:
		ss << skey << ">'" << sValue << "'";
		break;
	case EDCT_LT:
		ss << skey << "<'" << sValue << "'";
		break;
	case EDCT_GTE:
		ss << skey << ">='" << sValue << "'";
		break;
	case EDCT_LTE:
		ss << skey << "<='" << sValue << "'";
		break;
	case EDCT_NE:
		ss << skey << "!='" << sValue << "'";
		break;
	default:
		break;
	}
}

void MysqlDb::MakeOpType(stringstream& ss, const std::string& skey
	, const int64& sValue, eDBCalType eType)
{
	switch (eType)
	{
	case EDCT_EQ:
		ss << skey << "='" << sValue << "'";
		break;
	case EDCT_GT:
		ss << skey << ">'" << sValue << "'";
		break;
	case EDCT_LT:
		ss << skey << "<'" << sValue << "'";
		break;
	case EDCT_GTE:
		ss << skey << ">='" << sValue << "'";
		break;
	case EDCT_LTE:
		ss << skey << "<='" << sValue << "'";
		break;
	case EDCT_NE:
		ss << skey << "!='" << sValue << "'";
		break;
	default:
		break;
	}
}

bool MysqlDb::UpdateDBFile(const char* pszFileName, const char* pszDBName
	, int iDBId, const char* pszTableName)
{
	assert(pszFileName);
	assert(pszDBName);
	m_strName = pszDBName;
	std::vector<std::string> vecSqlData;
	if (!OpenCreateDBFile(pszFileName, vecSqlData, iDBId, pszTableName))
	{
		Log_Error("open %s file error", pszFileName);
		return false;
	}
	return DBExec(vecSqlData);
}

bool MysqlDb::UpdateLogFile(const char* pszFileName, const std::string& strDate)
{
	assert(pszFileName);
	std::vector<std::string> vecSqlData;
	if (!OpenLogDBFile(pszFileName, vecSqlData, strDate))
	{
		Log_Error("open %s file error", pszFileName);
		return false;
	}
	return DBExec(vecSqlData);
}

bool MysqlDb::IsNetError(int iErr)
{
	if (iErr == 1042 || iErr == 1043
		|| iErr == 1081 || (iErr >= 1158 && iErr <= 1162)
		|| iErr >= 2000 || iErr == 1218 || iErr == 1430)
		return true;
	return false;
}

bool MysqlDb::OpenCreateDBFile(const std::string& strFileName, std::vector<std::string>& vecCreateSql
	, int iDBId, const std::string& pszTableName)
{
	std::string strName = get_local_path(strFileName.c_str());
	FILE* pFile = fopen(strName.c_str(), "rb");
	if (NULL == pFile)
	{
		Log_Error("open %s file error", strName.c_str());
		return false;
	}

	//get file size
	fseek(pFile, 0, SEEK_END);
	int iFileLen = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char* pszBuff = new char[iFileLen + 1];
	memset(pszBuff, 0, iFileLen + 1);

	char* pszContent = new char[iFileLen + 256];
	memset(pszContent, 0, iFileLen + 256);

	fread(pszBuff, iFileLen, 1, pFile);
	fclose(pFile);

	if (!pszTableName.empty())
		sprintf_safe(pszContent, iFileLen + 128, pszBuff, m_strName.c_str()
			, m_strName.c_str(), iDBId, pszTableName);
	else
		sprintf_safe(pszContent, iFileLen + 128, pszBuff, m_strName.c_str()
			, m_strName.c_str(), iDBId);

	if (char* pDelim = strrchr(pszContent, ';'))
		*pDelim = '\0';

	str_split(pszContent, ';', vecCreateSql);
	delete[] pszContent;
	delete[] pszBuff;

	return true;
}

bool MysqlDb::OpenLogDBFile(const std::string& strFileName, std::vector<std::string>& vecCreateSql
	, const std::string& strDate)
{
	std::string strName = get_local_path(strFileName.c_str());
	FILE* pFile = fopen(strName.c_str(), "rb");
	if (NULL == pFile)
	{
		Log_Error("open %s file error", strName.c_str());
		return false;
	}

	// get file size
	fseek(pFile, 0, SEEK_END);
	int iFileLen = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char* pszBuff = new char[iFileLen + 1];
	memset(pszBuff, 0, iFileLen + 1);

	fread(pszBuff, iFileLen, 1, pFile);
	fclose(pFile);

	std::string strReplace;
	strReplace.assign(pszBuff, iFileLen);

	str_replace(strReplace, "[date]", strDate);

	str_split(strReplace, ';', vecCreateSql, true);
	delete[] pszBuff;

	return true;
}
