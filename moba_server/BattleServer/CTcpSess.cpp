#include "CTcpSess.h"
#include "CBattleUser.h"

CTcpSess::CTcpSess()
{
}

CTcpSess::~CTcpSess()
{
}

bool CTcpSess::Send_Msg(google::protobuf::Message* pMsg, int iCmd
	, int iModule/* = MsgModule::Battle*/, ResultCode eCode /* = Code_Common_Success*/
	, int iSeqid/* = 0*/)
{
	return svr_session::Send_Msg(pMsg, iCmd, iModule, eCode, iSeqid);
}

bool CTcpSess::Send_Msg(const std::string& strMsg, int iCmd
	, int iModule/* = MsgModule::Battle*/, ResultCode eCode/* = Code_Common_Success*/
	, int iSeqid/* = 0*/)
{
	return svr_session::Send_Msg(strMsg, iCmd, iModule, eCode, iSeqid);
}

bool CTcpSess::Send_Msg(const uchar* pBuf, size_t uiSize, int iCmd
	, int iModule/* = MsgModule::Battle*/, ResultCode eCode/* = Code_Common_Success*/
	, int iSeqid/* = 0*/)
{
	return svr_session::Send_Msg(pBuf, uiSize, iCmd, iModule, eCode, iSeqid);
}

bool CTcpSess::Send(const tagMsgHead* pMsg)
{
	return svr_session::Send(pMsg);
}

bool CTcpSess::InitTcpUdp(int64_t llCreate)
{
	return true;
}

void CTcpSess::Tick(int64_t llTick)
{
}

bool CTcpSess::IsOut()
{
	if (IsTimeOut()) return true;
	return CTcpUdpSession::IsOut();
}

bool CTcpSess::IsSameConn(const tagHostHd& stHead)
{
	return get_hd().m_connOid == stHead.m_connOid
		&& get_hd().m_threadOid == stHead.m_threadOid
		&& get_hd().m_type == stHead.m_type;
}

void CTcpSess::handle_msg(const tagMsgHead* pNetMsg)
{
	m_iSequence = pNetMsg->uiSeqid;
	m_llLastRecvTime = GetCurrTime();

	if (m_pBattleUser != nullptr)
		m_pBattleUser->handle_msg(pNetMsg);
}

void CTcpSess::on_disconnect()
{
	m_bStoped = true;
	if (m_pBattleUser != nullptr)
		m_pBattleUser->on_disconnect();
}













