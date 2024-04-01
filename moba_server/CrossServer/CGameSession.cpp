#include "CGameSession.h"
#include <sstream>
#include "log_mgr.h"
#include "CUserCacheInfo.h"
#include "CSessionMgr.h"

CGameSession::CGameSession()
{
}

CGameSession::~CGameSession()
{
}

void CGameSession::on_disconnected()
{
	std::stringstream ss;

	//UserOffline();
	Log_Warning("game server %u disconnect!", m_uiServerId);
	GAME_SESSIONS_INS->DelGameSvr(this);
}

void CGameSession::handle_msg(const tagMsgHead* pMsg)
{
	return;
}


