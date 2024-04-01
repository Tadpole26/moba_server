#pragma once
#include "server_pool.h"
#include "svr_client.h"

class CGameSession : public svr_session
{
public:
	CGameSession();
	~CGameSession();

	virtual void handle_msg(const tagMsgHead* pNetMsg);
	virtual void on_disconnect();
	virtual void on_connect();

private:
	void OnGetUserInfo(unsigned char* pMsg, size_t uiLen);
	void OnCreateUser(unsigned char* pMsg, size_t uiLen);
};

extern ServerPool<CGameSession> gSvrManager;