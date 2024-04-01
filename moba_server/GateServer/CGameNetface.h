#pragma once

#include "google/protobuf/message.h"
#include "svr_client.h"

class CGamePlayer;
class CGameNetface : public svr_reconn
{
public:
	CGameNetface();
	~CGameNetface();
private:
	virtual void handle_msg(const tagMsgHead* pMsg);
	//连接GameServer成功回调
	virtual void on_connect();
	//断开GameServer成功回调
	virtual void on_disconnect();
	//定時器
	virtual void trigger();
public:
	void HandleServerCommon(const tagMsgHead* pMsg);

	void HandleGameModule(const tagMsgHead* pNetMsgHead);
	//转发消息到GameServer
	void OnAccountLoginRet(const tagMsgHead* pNetMsgHead);
	void OnCreateRoleRet(const tagMsgHead* pNetMsgHead);
	void OnKickPlayer(const tagMsgHead* pNetMsgHead);
	void OnNoticePush(const tagMsgHead* pNetMsgHead);
};
