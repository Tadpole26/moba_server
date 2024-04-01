#pragma once

#include "svr_client.h"
#include "logic_interface.h"
#include "server_pool.h"

class CServerSession : public svr_session
{
public:
	CServerSession();
	~CServerSession();

	virtual void handle_msg(const tagMsgHead* pNetMsg);
	virtual void on_disconnect();
};

extern ServerPool<CServerSession> g_SvrMgr;
