#include "CUdpSess.h"
#include "CUdpLogic.h"
#include "CBattleUser.h"
#include "CBattleLogic.h"
#include "CUdpSess.h"

uint64_t CUdpSess::EndpointToI(const ip::udp::endpoint& oEp)
{
	uint64_t ullAddrI = oEp.address().to_v4().to_ulong();
	uint32_t uiPort = oEp.port();
	return (ullAddrI << 32) + uiPort;
}

CUdpSess::CUdpSess()
{
}

CUdpSess::~CUdpSess()
{
	if (m_pKcp != nullptr)
	{
		ikcp_release(m_pKcp);
		m_pKcp = nullptr;
	}

	m_bInit = false;
	m_pUdpLogic = nullptr;
	m_llRecvTimer = GetCurrTime();
	m_iRecvCount = 0;
	m_iRecvAll = 0;
	m_iTickMill = 0;
	m_bStoped = true;
	m_llPointKey = 0;
	m_llPointKeyPre = 0;
}

bool CUdpSess::Send_Msg(google::protobuf::Message* pMsg, int iCmd, int iModule, ResultCode eCode, int iSeqid)
{
	if (pMsg == nullptr)
	{
		Log_Error("module:%u, cmd:%d, class:%s", iModule, iCmd, typeid(*this).name());
		return false;
	}
	tagMsgHead* pNetMsgHead = MakeHeadMsg(pMsg, iModule, iCmd, iSeqid, eCode);
	if (!pNetMsgHead)
	{
		Log_Error("pNetMsgHead is nullptr, module:%u, cmd:%u, class:%s", iModule, iCmd, typeid(*this).name());
		return false;
	}
	bool bRet = Send(pNetMsgHead);
	return bRet;
}

bool CUdpSess::Send_Msg(const std::string& strMsg, int iCmd, int iModule, ResultCode eCode, int iSeqid)
{
	return Send_Msg((const uchar*)strMsg.c_str(), strMsg.size(), iCmd, iModule, eCode, iSeqid);
}

bool CUdpSess::Send_Msg(const uchar* pBuf, size_t uiSize, int iCmd, int iModule, ResultCode eCode, int iSeqid)
{
	tagMsgHead* pNetMsgHead = MakeHeadMsg((const char*)pBuf, uiSize, iModule, iCmd, iSeqid, eCode);
	if (!pNetMsgHead)
	{
		Log_Error("pNetMsgHead is nullptr! module:%u, cmd:%u, class:%s", iModule, iCmd, typeid(*this).name());
		return false;
	}
	bool bRet = Send(pNetMsgHead);
	return bRet;
}

bool CUdpSess::Send(const tagMsgHead* pMsg)
{
	if (IsStoped()) return true;
	
	SendKcpMsg((const char*)pMsg, pMsg->uiLen);

	return true;
}

void CUdpSess::Tick(int64_t llTick)
{
	if (IsStoped()) return;

	if (IsOut())
	{
		Log_Error("timer out!!!");
		on_disconnect();
		return;
	}

	if (m_pKcp != nullptr)
	{
		int iTickml = TickMill(llTick);
		if (iTickml >= m_iTickMill)
		{
			ikcp_update(m_pKcp, iTickml);

			if (m_pKcp != nullptr)
				m_iTickMill = ikcp_check(m_pKcp, iTickml);
			else
				m_iTickMill = 0;

			if (m_pKcp != nullptr)
			{
				if (TIME_OUT_SEC < 20 && ikcp_state(m_pKcp) != 0)
				{
					Log_Error("resend max times !!");
					on_disconnect();
					return;
				}
				CheckRecv();
			}
		}
	}
}

CUdpLogic* CUdpSess::GetUdpLogic() const
{
	return m_pUdpLogic;
}

void CUdpSess::SetUdpLogic(CUdpLogic* pVal)
{
	m_pUdpLogic = pVal;
}

bool CUdpSess::IsPrePoint(ip::udp::endpoint oEpoint)
{
	if (m_llPointKeyPre == 0) return false;
	uint64 ullKey = EndpointToI(oEpoint);
	if (ullKey == m_llPointKeyPre) return true;
	return false;
}

void CUdpSess::InitEndPoint(const ip::udp::endpoint& oEndPoint)
{
	m_llLastRecvTime = GetCurrTime();
	m_oUdpPoint = oEndPoint;
	m_iTickMill = 0;
	uint64 ullKey = EndpointToI(m_oUdpPoint);
	if (ullKey != m_llPointKey)
	{
		if (m_llPointKey != 0)
			m_llPointKeyPre = m_llPointKey;
		m_iPingCnt = 0;
		m_llPointKey = ullKey;
	}
}

void CUdpSess::Input(char* pUdpData, size_t uiBytesRcvd)
{
	if (IsStoped()) return;

	if (m_pKcp == nullptr) return;

	m_llLastRecvTime = GetCurrTime();
	m_iTickMill = 0;

	ikcp_input(m_pKcp, pUdpData, (long)uiBytesRcvd);
}

void CUdpSess::CheckRecv()
{
	if (IsStoped()) return;

	while (true)
	{
		char pKcpBuf[4096] = "";
		if (m_pKcp == nullptr) return;

		int iKcpRcvdBytes = ikcp_recv(m_pKcp, pKcpBuf, 4096);
		if (iKcpRcvdBytes <= 0)
		{
			if (iKcpRcvdBytes == -3)
				Log_Error("size is error!!! code:%d", iKcpRcvdBytes);
			return;
		}
		else
		{
			//recv
			if (iKcpRcvdBytes < (int)(NET_HEAD_SIZE))
			{
				Log_Error("%d", iKcpRcvdBytes);
				break;
			}

			tagMsgHead* pHead = (tagMsgHead*)pKcpBuf;
			if ((pHead->uiFlag >> 12) != 0x8)
			{
				Log_Error("%d", iKcpRcvdBytes);
				break;
			}

			if ((uint32)iKcpRcvdBytes < pHead->uiLen || pHead->uiLen < NET_HEAD_SIZE || pHead->uiLen > MSG_MAX_LEN)
			{
				Log_Error("%d", iKcpRcvdBytes);
				break;
			}

			if (!AddRecvTimes())
			{
				on_disconnect();
				return;
			}

			if (m_pBattleUser != nullptr)
				m_pBattleUser->handle_msg(pHead);
		}
	}
}

bool CUdpSess::InitTcpUdp(int64 llCreate)
{
	if (IsStoped()) return false;

	if (m_pKcp != nullptr)
	{
		ikcp_release(m_pKcp);
		SetInit(false);
		m_pKcp = nullptr;
	}

	m_pKcp = ikcp_create(GetConvId(), (void*)this);
	m_pKcp->output = &CUdpSess::UdpOutPut;

	//启动快速模式
	//第二个参数	nodelay-启用以后若干常规加速将启动
	//第三个参数	interval为内部处理时钟,默认设置为 10ms
	//第四个参数	resend为快速重传指标,设置为2
	//第五个参数	为是否禁用常规流控,这里禁止
	//ikcp_nodelay(m_pKcp, 1, 10, 2, 1);
	//设置成1次ACK跨越直接重传,这样反应速度会更快.内部时钟5毫秒
	ikcp_nodelay(m_pKcp, 1, 10, 2, 1);
	ikcp_setmtu(m_pKcp, MAX_UDP_BUFF);
	ikcp_wndsize(m_pKcp, 400, 400);

	m_llRecvTimer = GetCurrTime();
	m_iRecvCount = 0;
	m_iRecvAll = 0;
	m_iResetTimes = 0;
	m_iPingCnt = 0;
	m_llCreateMs = llCreate;
	return true;
}

void CUdpSess::on_disconnect()
{
	if (m_pUdpLogic != nullptr)
		m_pUdpLogic->RemoveOnline(this);

	m_bInit = false;
	if (m_pKcp != nullptr)
		ikcp_flush(m_pKcp);

	m_llRecvTimer = 0;
	m_iRecvAll = 0;
	m_iTickMill = 0;
	m_bStoped = true;
	m_llPointKey = 0;
	m_llPointKeyPre = 0;
	m_iPingCnt = 0;
	if (m_pBattleUser != nullptr)
		m_pBattleUser->on_disconnect();
}

void CUdpSess::Kill(int64 llTick)
{
	if (m_pBattleUser != nullptr)
		m_pBattleUser->Kill(llTick);
}

int CUdpSess::UdpOutPut(const char* pBuf, int len, ikcpcb* pKcp, void* pUser)
{
	((CUdpSess*)pUser)->SendUdpPackage(pBuf, len);
	return 0;
}

bool CUdpSess::SendUdpPackage(const char* pBuf, int len)
{
	if (IsStoped()) return false;

	if (m_pUdpLogic != nullptr && m_pUdpLogic->SendUdp(pBuf, len, m_oUdpPoint))
		return true;

	return false;
}

void CUdpSess::SendKcpMsg(const char* pBuf, int len)
{
	if (IsStoped()) return;

	if (m_pKcp == nullptr) return;

	m_iTickMill = 0;
	int iSendRet = ikcp_send(m_pKcp, pBuf, len);
	if (iSendRet < 0)
	{
		Log_Error("send_ret<0:%d, len:%d", iSendRet, len);
		Kill(GetMillSec());
	}
}

bool CUdpSess::AddRecvTimes()
{
	++m_iRecvAll;
	if (m_llRecvTimer != GetCurrTime())
	{
		m_llRecvTimer = GetCurrTime();
		m_iRecvCount = 0;
		return true;
	}
	else
	{
		++m_iRecvCount;
	}
	if (m_iRecvCount >= MAX_TECV_TIMES)
		return false;

	return true;
}

int CUdpSess::GetAllRecv()
{
	return m_iRecvAll;
}

std::string CUdpSess::GetIpStr()
{
	return m_oUdpPoint.address().to_string();
}
