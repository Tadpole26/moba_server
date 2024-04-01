#pragma once
#include "../protocol/result_code.pb.h"
#include "../network/msg_parser.h"

class CUser;
class svr_reconn;
class CPlayerProxy
{
public:
	static void CheckingSessionFromLoginSvr(CUser* pPlayer);
	
	static void LogicToGameServer(CUser* pPlayer, uint32_t uiGameId);

	static void NotifyPlayerOnline(CUser* pPlayer);
	static void NotifyPlayerOffline(CUser* pPlayer);

	static void Handle(CUser* pPlayer, const tagMsgHead* pNetMsgHead);
	static void HandleLoginModule(CUser* pPlayer, const tagMsgHead* pNetMsgHead);

	static void SendToGameSvr(CUser* pPlayer, const tagMsgHead* pNetMsgHead);

	static void OnCreatePlayer(CUser* pPlayer, const tagMsgHead* pNetMsgHead);
	static void OnRollName(CUser* pPlayer, const tagMsgHead* pNetMsgHead);

	static void NotifyErrorCode(CUser* pPlayer, msg_id_t cModuleId, msg_id_t cCmdId
		, ProtoMsg::ResultCode eCode, uint32_t uiReqId);

	static void TransmitToServer(svr_reconn* pConn, CUser* pPlayer, const tagMsgHead* pMsg);
	static void TransimitToClient(const tagMsgHead* pMsg);
};
