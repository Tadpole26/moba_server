#pragma once

#include "logic_interface.h"
#include "svr_client.h"
#include "server_pool.h"

class CUser;
class CSession : public svr_session
{
public:
	CSession();
	~CSession();

	virtual void handle_msg(const tagMsgHead* pNetMsg);
	virtual void on_disconnect();

private:
	void HandleInnerModule(const tagMsgHead* pMsg);

	void OnRecordOnlineNum(const tagMsgHead* pMsg);
	void OnClearOnlineNum(const tagMsgHead* pMsg);
	void OnNotifyOnline(const tagMsgHead* pMsg);
	void OnNotifyLogout(const tagMsgHead* pMsg);
	void OnPlayerLogin(const tagMsgHead* pMsg);
	void OnGetUniqueId(const tagMsgHead* pMsg);
	void OnWMsg(const std::string& strClient
		, const std::string& strServer
		, const std::string& strMsg);
	void OnModifyRoleName(const tagMsgHead* pMsg);
	void OnUpdatePlayerInfo(const tagMsgHead* pMsg);
	void OnUpdatePlayerInfoMb(const tagMsgHead* pMsg);

	static bool HandleOnlinePlayer(CUser* pPlayer);
	static bool HandleKickPlayer(CUser* pPlayer);
	static bool KickPlayerFromGate(CUser* pPlayer);

public:
	static void SendCreateRoleRet(int64 llPlayerId,
		uint32 uiGameId, std::string strRoleName);
	static void SendPlayerLoginRet(int64 llPlayerId, ProtoMsg::ResultCode eCode
		, uint32 uiGateId, int iProvince, std::string strProvince
		, std::string strSessionId, uint32 uiGmLevel, std::string strDevId, uint32 uiProArea);
	static void SendModifyRoleNameRet(int64 llPlayerId, uint32 uiGameId
		, std::string strRoleName, ProtoMsg::ResultCode eCode);
};

extern ServerPool<CSession> g_SvrMgr;
