#include "CDBProxy.h"
#include "CDBInstance.h"

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