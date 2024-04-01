#pragma once
#include "db_struct.h"

class CDBUser;
class CDBProxy
{
public:
	CDBProxy();
	~CDBProxy();

	bool Init();
	bool InitUser(CDBUser* pUser);
	bool LoadUserDataFromDB(int64_t llUserId, int& iRes);

private:
	sDBSelectReq m_oUserSelect;
};

#define gDBProxy Singleton<CDBProxy>::getInstance()