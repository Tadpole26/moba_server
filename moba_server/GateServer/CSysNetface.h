#pragma once

#include "google/protobuf/message.h"
#include "svr_client.h"

class CSysNetface : public svr_reconn
{
public:
	CSysNetface();
	~CSysNetface();

	void UpdateOnlinePlayerNum();

	void CleanOnlineNum(uint32_t uiSvrId, uint32_t uiAreaNo);
	void PlayerLogin(int64 llPlayerId, std::string strSession);
	void NotifyPlayerOnline(uint32_t uiSvrId, int64 llPlayerId);
	void NotifyPlayerOffline(uint32_t uiSvrId, int64 llPlayerId);

	virtual void trigger();
	void SendWChatMsg(const std::string& strClient,
		const std::string& strServer, const std::string& strMsg);

	void handle_msg(const tagMsgHead* pMsg);

private:
	void UpdateOnlinePlayerNum(uint32_t uiSvrId, uint32_t uiOnline, uint32_t uiWaitNum,
		std::string strIp, uint32 uiPort, uint32_t uiAreaNo);

	//virtual void hand_msg(const tagMsgHead* pMsg);
	virtual void on_connect();
	virtual void on_disconnect();

	void HandleServerCommon(const tagMsgHead* pMsgHead);
	void HandleSysLogModule(const tagMsgHead* pMsgHead);

	void OnPlayerLoginRet(const tagMsgHead* pMsgHead);
	void OnKickPlayer(const tagMsgHead* pMsgHead);
};
