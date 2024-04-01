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
	//����GameServer�ɹ��ص�
	virtual void on_connect();
	//�Ͽ�GameServer�ɹ��ص�
	virtual void on_disconnect();
	//���r��
	virtual void trigger();
public:
	void HandleServerCommon(const tagMsgHead* pMsg);

	void HandleGameModule(const tagMsgHead* pNetMsgHead);
	//ת����Ϣ��GameServer
	void OnAccountLoginRet(const tagMsgHead* pNetMsgHead);
	void OnCreateRoleRet(const tagMsgHead* pNetMsgHead);
	void OnKickPlayer(const tagMsgHead* pNetMsgHead);
	void OnNoticePush(const tagMsgHead* pNetMsgHead);
};
