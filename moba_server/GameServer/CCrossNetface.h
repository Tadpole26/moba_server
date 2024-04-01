#pragma once
#include "svr_client.h"

class CCrossNetface : public svr_reconn
{
public:
	CCrossNetface();
	~CCrossNetface();

	void HandleServerCommon(const tagMsgHead* pMsgHead);
	void HandleCrossModule(const tagMsgHead* pMsgHead);
	void HandleTransmitData(const tagMsgHead* pMsgHead);

private:
	virtual void handle_msg(const tagMsgHead* pNetMsg);
	virtual void on_connect();
	virtual void on_disconnect();
	virtual void trigger();
}; 
