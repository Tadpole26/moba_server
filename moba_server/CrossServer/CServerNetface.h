#pragma once

#include "global_define.h"
#include "server_logic_face.h"

class CSessionMgr;
class CClientLogic : public CSvrLogicFace
{
public:
	CClientLogic();
	~CClientLogic();

	void handle_logic_msg(const tagNetMsg* pNetMsg);
	void HandleServerCommon(const tagMsgHead* pMsgHead, const tagHostHd& hd);

private:
	void OnServerRegister(const tagMsgHead* pMsgHead, const tagHostHd& hd);
	//std::set<uint32_t> m_setGameGroup;
};
