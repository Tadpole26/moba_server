#include "CDBProxy.h"
#include "CDBInstance.h"
#include "CDBUser.h"

CDBProxy::CDBProxy()
{
}

CDBProxy::~CDBProxy()
{
}

bool CDBProxy::Init()
{
	m_oUserSelect.Init("u_user");
	return true;
}

bool CDBProxy::InitUser(CDBUser* pUser)
{
	return true;
}

bool CDBProxy::InitUser(sDBRet& oRet, CDBUser* pUser)
{
	//在CheckUserBase中已经加载,所以可以直接使用,无需再次LOAD
	ERROR_LOG_EXIT0(CBaseInfoDBPkg::ReadDBBaseInfo(oRet.Ret_Vec(o_db_baseinfo::tb_name), pUser));
	return true;

Exit0:
	return false;
}

bool CDBProxy::LoadUserDataFromDB(int64_t llUserId, int& iRes)
{
	iRes = 0;
	m_oUserSelect.AddCon("_id", llUserId);
	if (!gDBInstance->QueryData(m_oUserSelect))
	{
		iRes = 1;
		return false;
	}

	if (m_oUserSelect.empty_ret())
	{
		iRes = 2;
		return false;
	}
	return true;
}