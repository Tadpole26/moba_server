#include "CDBInstance.h"
#include "CDBLogic.h"
#include "log_mgr.h"

CDBInstance::CDBInstance()
{
}


CDBInstance::~CDBInstance()
{
}

bool CDBInstance::Init()
{
	bool bRet = false;

	for (size_t i = 0; i < DB_THREAD_NUM; ++i)
	{
		bRet = m_oQueryThread[i].Init(gDBLogic->m_oConstCfg.m_strMongoHost
		, gDBLogic->m_oConstCfg.m_strMongoUser, gDBLogic->m_oConstCfg.m_strMongoPasswd
		, gDBLogic->m_oConstCfg.m_strMongoGameDb, gDBLogic->m_oConstCfg.m_strMongoAuth);
		if (!bRet) return false;
	}

	bRet = m_oQuery.Init(gDBLogic->m_oConstCfg.m_strMongoHost
		, gDBLogic->m_oConstCfg.m_strMongoUser, gDBLogic->m_oConstCfg.m_strMongoPasswd
		, gDBLogic->m_oConstCfg.m_strMongoGameDb, gDBLogic->m_oConstCfg.m_strMongoAuth);

	if (!bRet) return false;

	bRet = m_oMailQuery.Init(gDBLogic->m_oConstCfg.m_strMongoHost
		, gDBLogic->m_oConstCfg.m_strMongoUser, gDBLogic->m_oConstCfg.m_strMongoPasswd
		, gDBLogic->m_oConstCfg.m_strMongoMailDb, gDBLogic->m_oConstCfg.m_strMongoAuth, false);

	if (!bRet) return false;

	return true;
}

void CDBInstance::UnInit()
{
	for (size_t i = 0; i < DB_THREAD_NUM; i++)
	{
		m_oQueryThread[i].UnInit();
	}
}

void CDBInstance::OnTimer()
{
	static uint32 uiCount = 0;
	if (++uiCount >= 30)
	{
		m_oQuery.DbPing();
		m_oMailQuery.DbPing();
		uiCount = 0;
	}

	static uint32 uiFailedTimer = 0;
	if (++uiFailedTimer >= 5)
	{
		uint32 uiFailedNum = 0;
		for (size_t i = 0; i < DB_THREAD_NUM; ++i)
		{
			uiFailedNum += m_oQueryThread[i].GetFailedNum();
		}

		if (uiFailedNum >= 5)
		{
			//¶à´ÎÊ§°Ü¶ÏµôÍø¹Ø
		}

		uiFailedTimer = 0;
	}
}

bool CDBInstance::QueryData(sDBSelectReq& stRequest)
{
	return m_oQuery.DBSelectOne(stRequest);
}

bool CDBInstance::QuerySelectData(sDBSelectReq& stRequest, fn_RetCall fnCall, size_t& uiRetNum)
{
	return m_oQuery.DBSelect(stRequest, fnCall, uiRetNum);
}

bool CDBInstance::QueryMultiData(sDBSelectReq& stRequest, fn_RetCall fnCall, size_t& uiRetNum
	, int64 llBeginId, int32 iNum)
{
	return m_oQuery.DBSelectMore(stRequest, fnCall, uiRetNum, llBeginId, iNum);
}

int64 CDBInstance::QueryCount(sDBSelectReq& stRequest)
{
	return m_oQuery.DBCount(stRequest);
}

void CDBInstance::PushGame(sDBRequest& stRequest)
{
	m_oQueryThread[(stRequest.Con().NumULong("_id") / INDEX_MOD_NUM) % DB_THREAD_NUM].PushData(stRequest);
}

