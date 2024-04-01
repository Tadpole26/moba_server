#pragma once
#include "logic_interface.h"
#include "msg_interface.h"
#include "msg_parser.h"
#include "server_logic_face.h"


//管理所有连接上来的服务
class CClientLogic : public CSvrLogicFace
{
public:
	CClientLogic(void);
	~CClientLogic(void);
public:
	virtual void handle_logic_msg(const tagNetMsg* pNetMsg);
private:
	void HandleServerCommon(tagMsgHead* pMsgHead, const tagHostHd& hd);
	void OnRegisterServer(tagMsgHead* pMsgHead, const tagHostHd& hd);
};
