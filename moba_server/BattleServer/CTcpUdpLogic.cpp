#include "CTcpUdpLogic.h"
#include "CBattleLogic.h"

CTcpUdpLogic::CTcpUdpLogic()
{
}

CTcpUdpLogic::~CTcpUdpLogic()
{
}

CTcpUdpSession::CTcpUdpSession()
{
}

CTcpUdpSession::~CTcpUdpSession()
{
}

bool CTcpUdpSession::IsOut()
{
	if (GetCurrTime() > (m_llLastRecvTime + TIME_OUT_SEC))
		return true;
	return false;
}

int CTcpUdpSession::TickMill(int64_t llVal)
{
	return (int)(llVal - m_llCreateMs);
}

int CTcpUdpSession::AddResetTimes()
{
	if (m_llReset == 0)
		m_llReset = GetCurrTime();
	else if (m_llReset + 2 <= GetCurrTime())
	{
		m_llReset = GetCurrTime();
		m_iResetTimes = 0;
	}
	return ++m_iResetTimes;
}
