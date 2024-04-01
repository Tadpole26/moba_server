#pragma once
#include "svr_client.h"

class CGameSession : public svr_session
{
public:
	CGameSession();
	~CGameSession();

	virtual void on_disconnected();
	virtual void handle_msg(const tagMsgHead* pMsg);

	//void OnUserOnline(user_cache_info_ptr_type pUser, const std::string& strMsg);
	//void OnUserOffline(user_cache_info_ptr_type pUser);
	//void UserOffline();
public:
	uint32 m_uiPlatId = 0;
	uint32 m_uiIndex = 0;
	uint32 m_uiGroupId = 0;
	std::string m_strExIp = "";
};