#pragma once
#include "util_time.h"
#include "CBaseInfoDBPkg.h"

namespace ProtoMsg
{
	class Msg_ServerDB_GD_CreateUser_Req;
}

class CDBUser
{
public:
	CDBUser();
	~CDBUser();

	void Init();
	void Release();

	bool GetOnline() { return m_bOnline; }
	void SetOnline(bool bValue) { m_bOnline = bValue; }

	int64_t GetUserId() { return m_oBaseInfoPkg.GetBaseInfo().m_llUserId; }

	bool AddNewUser(const ProtoMsg::Msg_ServerDB_GD_CreateUser_Req& oCreateReq);
public:
	time_t m_llFix = GetCurrTime();
	bool m_bOnline = false;

	CBaseInfoDBPkg m_oBaseInfoPkg;
};
