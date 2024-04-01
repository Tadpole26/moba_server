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
	void ReqUniqueId(int64 llPlayerId, const std::string& strPlayerName, uint32 uiServerId);
	void ModifyPlayerName(int64 llPlayerId, const std::string& strPlayerName
		, uint32 uiServerId);
	//void UpdatePlayerInfo(CGamePlayer* pPlayer, bool bLogin);
	//void UpdatePlayerMb(int64 llPlayerId, uint32 uiRmb, uint32 uiRecharge);

private:
	virtual void handle_msg(const tagMsgHead* pNetMsg);
	virtual void on_connect();
	virtual void on_disconnect();
};