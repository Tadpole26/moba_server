#pragma once

#include <list>
#include <string>
#include "server_logic_face.h"

class CClientLogic : public CSvrLogicFace
{
public:
	CClientLogic();
	~CClientLogic();
public:
	virtual void handle_logic_msg(const tagNetMsg* pNetMsg);
private:
	void HandleServerCommon(tagMsgHead* pMsgHead, const tagHostHd& hd);
	void OnRegisterServer(tagMsgHead* pMsgHead, const tagHostHd& hd);
};