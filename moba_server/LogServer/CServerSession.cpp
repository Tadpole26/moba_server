#include "CServerSession.h"
#include "log_mgr.h"

ServerPool<CServerSession> g_SvrMgr;

CServerSession::CServerSession()
{
	m_eServerKind = SERVER_KIND_NONE;
	m_uiServerId = 0;
}

CServerSession::~CServerSession()
{
}

void CServerSession::on_disconnect()
{
	Log_Warning("server kind:%u, id:%u disconnect!", m_eServerKind, m_uiServerId);
	g_SvrMgr.DelServer(m_eServerKind, m_uiServerId);
}

void CServerSession::handle_msg(const tagMsgHead* pNetMsg)
{
	if (!pNetMsg) return;
	uchar* pBuf = NET_DATA_BUF(pNetMsg);
	uint32 uiLen = NET_DATA_SIZE(pNetMsg);

	switch (pNetMsg->uiCmdId)
	{
	case 2:
		break;
	default:
		Log_Error("cmd:%u not found!", pNetMsg->uiCmdId);
		break;
	}
}