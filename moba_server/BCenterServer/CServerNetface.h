#pragma once
#include "logic_interface.h"
#include "msg_interface.h"
#include "msg_parser.h"
#include "server_logic_face.h"

//管理所有连接上来的服务(cross_server, battle_server)
class CClientLogic : public CSvrLogicFace
{
public:
	CClientLogic();
	~CClientLogic();

	void handle_logic_msg(const tagNetMsg* pNetMsg);
	void HandleBattleServerMsg(unsigned char* pMsg, size_t uiSize, uint32_t uiCmd, const tagHostHd& stHead);
	void HandleServerCommonMsg(tagMsgHead* pMsgHead, const tagHostHd& stHead);

private:
	void OnBattleServerRegister(unsigned char* pMsg, size_t uiSize, const tagHostHd& stHead);
	void OnCrossServerRegister(tagMsgHead* pMsgHead, const tagHostHd& stHead);
};
