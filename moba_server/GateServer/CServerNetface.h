#pragma once

#include "server_logic_face.h"


//�����������ӵ��û�
class CUser;
class CClientLogic : public CSvrLogicFace 
{
public:
	CClientLogic();
	~CClientLogic();

	virtual void handle_logic_msg(const tagNetMsg* pNetMsg);
	//��ͻ������ӶϿ�
	void OnDisconnect(CUser* pUser);
	void OnKickConnect(CUser* pUser, bool bSysKill);

private:
	void OnVerifyAccount(const tagMsgHead* pHeadMsg, const tagHostHd& hd);

};
