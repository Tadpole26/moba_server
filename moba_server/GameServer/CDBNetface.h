#pragma once

#include "svr_client.h"

class CDBNetface : public svr_reconn
{
public:
	CDBNetface();
	~CDBNetface();

protected:
	virtual void handle_msg(const tagMsgHead* pNetMsg);
	virtual void on_connect();
	virtual void on_disconnect();
	virtual void trigger();

private:
	void HandleServerCommon(const tagMsgHead* pMsg);
	void OnGetUserInfo(const tagMsgHead* pMsg);
	void OnCreateUser(const tagMsgHead* pMsg);
};
