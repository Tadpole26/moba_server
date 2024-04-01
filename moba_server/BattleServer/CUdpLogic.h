#pragma once

#include <boost/asio.hpp>
#include <thread>
#include "CTcpUdpLogic.h"
#include "msg_make.h"

class CRoomThread;
class CBattleUser;
using namespace boost::asio;
class CRoom;
class CUdpSess;

class CUdpLogic : public CTcpUdpLogic, public CMsgMake
{
public:
	struct tagConvItem
	{
		int64_t m_llRoomId = 0;
		int64_t m_llUserId = 0;
		CBattleUser* m_pBtlUser = nullptr;
	};

	CUdpLogic(int iPort, CRoomThread* pRoomNet);
	~CUdpLogic();

	static void InitKcp();
	virtual bool Init(int iPort, CRoomThread* pRoomNet);
	virtual void Run();

	virtual void KillUser(CBattleUser* pBtlUser);
	virtual void EnterUser(CBattleUser* pBtlUser);
	virtual void DeleteUser(CBattleUser* pBtlUser);
	virtual void AddRoom(CRoom* pRoom);
	virtual void RemoveRoom(CRoom* pRoom);

	virtual void Stop();
	virtual void Fini();
	virtual void OnTimer(int iTime, int64_t llTick);
	virtual void Tick(int64_t llTick);

	bool SendUdp(const char* pBuf, int iLen, const ip::udp::endpoint& oEndPoint);

	void SetOnline(CUdpSess* pUdp);
	void RemoveOnline(CUdpSess* pUdp);

private:
	tagMsgHead* Gethead(size_t uiBytesRecvd);
	void PushLinkResult(ResultCode eCode, int iReqId, int iTimes = 1);
	void SendEmptyBuf(const tagMsgHead* pMsg);

	CBattleUser* GetUserByConv(uint32_t uiConvId, int64_t& llRoomId, int64_t& llUserId);

	void HandleUdpReceiveFrom(const boost::system::error_code& oError, size_t uiBytesRecvd);
	void HookUdpAsyncReceive(void);
	void HandleKcpTimer(void);
	void HookKcpTimer(int64_t llMicroSec = TICK_MILL_SEC);
	void HandleKcpPacket(size_t uiBytesRecvd);

	CBattleUser* EnterRoomUdp(uint32_t uiConvId, int64_t llRoomId, int64_t llUserId);
	CBattleUser* OnRecvDataUser(uint32_t uiConvId, int64_t& llRoomId, int64_t& llUserId);
	
	void OnRecvData();
	void OnRecvDataInit(tagMsgHead* pHead);
	int AddTempEnd(ip::udp::endpoint& oEndPoint, int iTimes);
	bool IsEndRoom(int64_t llRoomId);
	void SendEndRoomResult(int64_t llRoomId);

private:
	boost::asio::io_service					m_oIoService;
	ip::udp::socket							m_oUdpSocket;
	boost::asio::deadline_timer				m_oKcpTimer;
	ip::udp::endpoint						m_oUdpEndPoint;
	int										m_iCurConvId = 0;
	char									m_arrUdpData[MAX_UDP_BUFF*2];
	size_t									m_uiBytesRecvd = 0;
	char									m_arrErrBuf[MAX_UDP_BUFF];
	
	CRoomThread*							m_pRoomNet = nullptr;

	std::thread								m_oThread;
	int										m_iPort = 0;
	bool									m_bStoped = false;
	std::map<uint32_t, tagConvItem>			m_mapConvToRoom;
	std::map<int64_t, int>					m_mapTempEnd;
	std::map<int64_t, CUdpSess*>			m_mapSessOnline;
	NumTimer<1000>							m_oTmrTempEnd;
	int64_t									m_llTickMs = 0;
	size_t									m_uiOutSize = 0;
	int64_t									m_llIndex = 0;
};