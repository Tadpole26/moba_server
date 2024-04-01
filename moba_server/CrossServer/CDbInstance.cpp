#include "CDbInstance.h"
#include "CCrossLogic.h"
#include "log_mgr.h"

CDbInstance::CDbInstance()
{
}


CDbInstance::~CDbInstance()
{
}

bool CDbInstance::Init()
{
	bool bRet = false;

	bRet = m_oQueryThread.Init(CROSS_LOGIC_INS->m_oConstCfg.m_strMongoHost
		, CROSS_LOGIC_INS->m_oConstCfg.m_strMongoUser, CROSS_LOGIC_INS->m_oConstCfg.m_strMongoPasswd
		, CROSS_LOGIC_INS->m_oConstCfg.m_strMongoGameName, CROSS_LOGIC_INS->m_oConstCfg.m_strMongoAuth);
	if (!bRet)
		goto Exit0;

	bRet = m_oQuery.Init(CROSS_LOGIC_INS->m_oConstCfg.m_strMongoHost
		, CROSS_LOGIC_INS->m_oConstCfg.m_strMongoUser, CROSS_LOGIC_INS->m_oConstCfg.m_strMongoPasswd
		, CROSS_LOGIC_INS->m_oConstCfg.m_strMongoGameName, CROSS_LOGIC_INS->m_oConstCfg.m_strMongoAuth);

	return true;
Exit0:
	return false;
}

void CDbInstance::UnInit()
{
	m_oQueryThread.UnInit();
}

void CDbInstance::OnTimer()
{
	static uint32 uiCount = 0;
	if (++uiCount >= 30)
	{
		m_oQuery.DbPing();
		uiCount = 0;
	}
}

bool CDbInstance::QueryData(sDBSelectReq& stRequest)
{
	return m_oQuery.DBSelectOne(stRequest);
}

bool CDbInstance::QueryMultiData(sDBSelectReq& stRequest, fn_RetCall fnCall, size_t& uiRetNum
	, int64 llBeginId, int32 iNum)
{
	return m_oQuery.DBSelectMore(stRequest, fnCall, uiRetNum, llBeginId, iNum);
}

int64 CDbInstance::QueryCount(sDBSelectReq& stRequest)
{
	return m_oQuery.DBCount(stRequest);
}

void CDbInstance::PushWrite(sDBRequest& stRequest)
{
	m_oQueryThread.PushData(stRequest);
}

bool CDbInstance::Exec(sDBRequest& stRequest)
{
	return m_oQuery.DBExec(stRequest);
}
