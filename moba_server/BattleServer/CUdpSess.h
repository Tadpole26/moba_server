#pragma once
#include "CTcpUdpLogic.h"
#include "msg_make.h"
#include "msg_common.pb.h"
#include <boost/asio.hpp>
#include "ikcp.h"

class CUdpLogic;
using namespace boost::asio;
class CUdpSess : public CTcpUdpSession, public CMsgMake
{
public:
	CUdpSess();
	~CUdpSess();

	static uint64_t EndpointToI(const ip::udp::endpoint& oEp);

	virtual bool Send_Msg(google::protobuf::Message* pMsg, int iCmd
		, int iModule = MsgModule::Battle, ResultCode eCode = Code_Common_Success
		, int iSeqid = 0);

	virtual bool Send_Msg(const std::string& strMsg, int iCmd
		, int iModule = MsgModule::Battle, ResultCode eCode = Code_Common_Success
		, int iSeqid = 0);

	virtual bool Send_Msg(const uchar* pBuf, size_t uiSize, int iCmd
		, int iModule = MsgModule::Battle, ResultCode eCode = Code_Common_Success
		, int iSeqid = 0);

	virtual bool Send(const tagMsgHead* pMsg);

	virtual bool IsTcp() { return false; }
	virtual void Tick(int64_t llTick);

	CUdpLogic* GetUdpLogic() const;
	void SetUdpLogic(CUdpLogic* pVal);

	ip::udp::endpoint GetUdpPoint() { return m_oUdpPoint; }
	int64_t GetUdpPointKey() { return m_llPointKey; }
	bool IsPrePoint(ip::udp::endpoint oEpoint);

	void InitEndPoint(const ip::udp::endpoint& oEndPoint);
	void Input(char* pUdpData, size_t uiBytesRcvd);
	void CheckRecv();

	virtual bool InitTcpUdp(int64 llCreate);
	void on_disconnect();
	void Kill(int64 llTick);
	static int UdpOutPut(const char* pBuf, int len, ikcpcb* pKcp, void* pUser);
	bool SendUdpPackage(const char* pBuf, int len);
	void SendKcpMsg(const char* pBuf, int len);

	bool IsInit() const { return m_bInit; }
	void SetInit(bool bVal) { m_bInit = bVal; }
	bool AddRecvTimes();
	int GetAllRecv();
	std::string GetIpStr();
protected:
	ikcpcb							*m_pKcp = nullptr;
	ip::udp::endpoint				m_oUdpPoint;
	int64_t							m_llPointKey = 0;
	int64_t							m_llPointKeyPre = 0;
	CUdpLogic						*m_pUdpLogic = nullptr;
	bool							m_bInit = false;
	time_t							m_llRecvTimer = GetCurrTime();
	int								m_iRecvCount = 0;
	int								m_iRecvAll = 0;
	int								m_iTickMill = 0;
};