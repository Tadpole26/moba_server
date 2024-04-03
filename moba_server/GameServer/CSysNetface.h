#pragma once
#include "svr_client.h"

class CSysNetface : public svr_reconn
{
public:
	CSysNetface();
	~CSysNetface();

	virtual void trigger();
	void SendWChatMsg(const std::string& strClient
		, const std::string& strServer, const std::string& strMsg);
	void ReqUniqueId(int64_t llUserId, const std::string& strUserName, uint32 uiServerId);
	void OnUniqueIdRsp(int64_t llUserId, const std::string strUserName, ProtoMsg::ResultCode eCode);
	void ModifyPlayerName(int64 llPlayerId, const std::string& strPlayerName
		, uint32 uiServerId);
	

private:
	virtual void handle_msg(const tagMsgHead* pNetMsg);
	virtual void on_connect();
	virtual void on_disconnect();
};