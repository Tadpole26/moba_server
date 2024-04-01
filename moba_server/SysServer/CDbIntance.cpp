#include "CDbInstance.h"
#include "log_mgr.h"
#include "CSysLogic.h"

CDbInstance::CDbInstance()
{
}

CDbInstance::~CDbInstance()
{
}

bool CDbInstance::Init()
{
	bool bRet = false;
	
	bRet = CreateDBTable();
	if (!bRet)
	{
		Log_Error("create db error");
		return false;
	}

	bRet = m_oMongoMgr.Init(SYS_LOGIC_INS->m_oConstCfg.m_strMongoHost
		, SYS_LOGIC_INS->m_oConstCfg.m_strMongoUser
		, SYS_LOGIC_INS->m_oConstCfg.m_strMongoPasswd
		, SYS_LOGIC_INS->m_oConstCfg.m_strMongoAccName
		, SYS_LOGIC_INS->m_oConstCfg.m_strMongoAuth);
	if (!bRet)
	{
		Log_Error("create mongo error");
		return false;
	}

	return true;
}

void CDbInstance::UnInit()
{
	m_oMongoMgr.UnInit();
	m_oMysql.DbDisConnect();

}

void CDbInstance::OnTime()
{
	if (m_oInternal.On())
	{
		m_oMysql.DBPing();
	}
}

bool CDbInstance::CreateDBTable()
{
	if (!m_oMysql.Init(SYS_LOGIC_INS->m_oConstCfg.m_strMysqlHost
		, SYS_LOGIC_INS->m_oConstCfg.m_strMysqlUser
		, SYS_LOGIC_INS->m_oConstCfg.m_strMysqlPasswd
		, ""
		, SYS_LOGIC_INS->m_oConstCfg.m_uiMysqlPort))
		return false;

	char szBuff[1024] = { 0 };
	sprintf_safe(szBuff, u8"gmws_uiq_rolename_%u", SYS_LOGIC_INS->m_oConstCfg.m_uiGroupId);
	m_strTableName = szBuff;

	sprintf_safe(szBuff, u8"CREATE DATABASE IF NOT EXISTS `%s`", SYS_LOGIC_INS->m_oConstCfg.m_strMysqlAccName.c_str());
	if (-1 == m_oMysql.DBExec(szBuff, eDB_None))
		return false;

	sprintf_safe(szBuff, u8"USE `%s`;", SYS_LOGIC_INS->m_oConstCfg.m_strMysqlAccName.c_str());
	if (-1 == m_oMysql.DBExec(szBuff, eDB_None))
		return false;

	m_oMysql.SetDbName(SYS_LOGIC_INS->m_oConstCfg.m_strMysqlAccName);

	sprintf_safe(szBuff, u8"call sp_create_uiq_rolename_table(%d);"
		, SYS_LOGIC_INS->m_oConstCfg.m_uiGroupId);
	return m_oMysql.DBCallProc(szBuff);
}

bool CDbInstance::ModifyRoleName(int64 llPlayerId, const std::string& strName)
{
	if (!IsNameValid(llPlayerId, strName))
		return false;

	static sDBRequest stRequest(eDBQueryType::eDB_Update, m_strTableName);
	stRequest.AddCon("`rol_id`", llPlayerId);
	stRequest.AddReqStr("`rol_name`", strName);
	if (m_oMysql.DBExec(stRequest) <= 0)
	{
		Log_Error("Role:%lld, modify role name failed!", llPlayerId);
		return false;
	}
	return true;
}

int CDbInstance::InsertNewRole(int64 llPlayerId, const std::string& strName, bool bTransfer)
{
	int iAccId = (int)(llPlayerId / INDEX_MOD_NUM);
	uint32 uiArea = SYS_LOGIC_INS->m_oConstCfg.m_uiGroupId;
	if (bTransfer)
	{
		uiArea = SPLIT_MOD_GAME(llPlayerId);
		sDBRequest stRequest(eDBQueryType::eDB_InsertUpdate, "gmws_uiq_rolename_" + to_string(uiArea));
		stRequest.AddCon("`rol_id`", llPlayerId);
		stRequest.AddReqStr("`rol_name`", strName);
		stRequest.AddReq("`rol_acc`", iAccId);
		stRequest.AddReq("`rol_area`", uiArea);
		stRequest.AddReqStr("`rol_create_time`", CurrTimeToString(VST_ALL));
		if (m_oMysql.DBExec(stRequest) <= 0)
		{
			Log_Custom("transfer_role", "_id:%lld insert mysql failed!", llPlayerId);
			return -1;
		}
	}
	else
	{
		if (!IsNameValid(llPlayerId, strName))
			return -2;

		static sDBRequest stRequest(eDBQueryType::eDB_InsertUpdate, m_strTableName);
		stRequest.AddCon("`rol_id`", llPlayerId);
		stRequest.AddReqStr("`rol_name`", strName);
		stRequest.AddReq("`rol_acc`", iAccId);
		stRequest.AddReq("`rol_area`", uiArea);
		stRequest.AddReqStr("`rol_create_time`", CurrTimeToString(VST_ALL));
		if (m_oMysql.DBExec(stRequest) <= 0)
		{
			Log_Error("Role:%lld, insert mysql failed!", llPlayerId);
			return -1;
		}
	}

	std::string strTable = "u_acc_" + to_string(iAccId % 20);
	static sDBRequest stMongoReq;
	stMongoReq.Init(eDB_InsertUpdate, strTable);
	stMongoReq.AddCon("_id", iAccId);
	stMongoReq.AddCon("id", uiArea);
	stMongoReq.AddReq("id",uiArea);
	stMongoReq.AddReq("level", 1);
	stMongoReq.AddReqStr("name", strName);
	stMongoReq.AddReq("roleid", llPlayerId);
	stMongoReq.AddReq("vip", 0);
	stMongoReq.AddReqTime("create_time", GetCurrTime());
	m_oMongoMgr.PushData(stMongoReq);
	return 0;
}

void CDbInstance::UpdateRoleInfo(int64 llPlayerId, const std::string& strName, uint32 uiLevel,
	uint32 uiVip, bool bLogin, int64 llLoginTime, int64 llLogoutTime)
{
	int iAccId = (int)(llPlayerId / INDEX_MOD_NUM);
	uint32 uiArea = SYS_LOGIC_INS->m_oConstCfg.m_uiGroupId;

	std::string strTable = "u_acc_" + to_string(iAccId % 20);
	static sDBRequest stMongoReq;
	stMongoReq.Init(eDB_InsertUpdate, strTable, "role", tagSecTblDesc::eST_List);
	stMongoReq.AddCon("_id", iAccId);
	stMongoReq.AddCon("id", uiArea);
	stMongoReq.AddReq("id", uiArea);
	stMongoReq.AddReq("roleid", llPlayerId);
	stMongoReq.AddReq("level", uiLevel);
	stMongoReq.AddReqStr("name", strName);
	stMongoReq.AddReq("vip", uiVip);
	stMongoReq.AddReqTime("update_time", GetCurrTime());
	if (bLogin)
	{
		stMongoReq.AddReqTime("login_time", GetCurrTime());
		if (!IsSameDay(GetCurrTime(), llLogoutTime))
		{
			stMongoReq.AddReqTime("login_time", GetCurrTime());
			if (!IsSameDay(GetCurrTime(), llLogoutTime))
			{
				sDBRequest incReq(eDB_InsertUpdate, strTable, "role", tagSecTblDesc::eST_List);
				incReq.AddCon("_id", iAccId);
				incReq.AddCon("id", uiArea);
				incReq.AddReq("online_time", 0);
				m_oMongoMgr.PushData(incReq);
			}
		}
	}
	else
	{
		stMongoReq.AddReqTime("logout_time", GetCurrTime());

		sDBRequest incReq(eDB_IncValue, strTable, "role", tagSecTblDesc::eST_List);
		incReq.AddCon("_id", iAccId);
		incReq.AddCon("id", uiArea);
		incReq.AddReq("online_time", (int)(llLogoutTime - llLoginTime));
		m_oMongoMgr.PushData(incReq);
	}

	m_oMongoMgr.PushData(stMongoReq);
}

void CDbInstance::UpdateRoleInfoMB(int64 llPlayerId, uint32 uiRmb, uint32 uiRecharge)
{
	int iAccId = (int)(llPlayerId / INDEX_MOD_NUM);
	uint32 uiArea = SYS_LOGIC_INS->m_oConstCfg.m_uiGroupId;

	std::string strTable = "u_acc_" + to_string(iAccId % 20);
	static sDBRequest stMongoReq;
	stMongoReq.Init(eDB_Update, strTable, "role", tagSecTblDesc::eST_List);
	stMongoReq.AddCon("_id", iAccId);
	stMongoReq.AddCon("id", uiArea);
	stMongoReq.AddReq("id", uiArea);
	stMongoReq.AddReq("roleid", llPlayerId);
	stMongoReq.AddReq("rmb", uiRmb);
	stMongoReq.AddReq("recharge", uiRecharge);

	m_oMongoMgr.PushData(stMongoReq);
}

bool CDbInstance::GetUserLoginInfo(int64& llPlayerId, int& iProvince, std::string& strProvince
	, std::string& strSession, uint32& uiGmLevel, std::string& strDev
	, uint32& uiArea, std::string& strChannel, std::string& accName, std::string& strCityCode)
{
	//从mongo中获取session key
	return true;
}

bool CDbInstance::IsNameValid(int64 llPlayerId, const std::string& strName)
{
	static sDBSelectReq selReq(m_strTableName);
	selReq.AddConStr("`rol_name`", strName);
	selReq.AddField("`rol_id`");
	bool bRes = m_oMysql.DBSelect_One(selReq);
	int64 llOldId = selReq.Ret_Main().NumLong("rol_id");
	if (!bRes || (llOldId != 0 && llOldId != llPlayerId))
		return false;

	return true;
}