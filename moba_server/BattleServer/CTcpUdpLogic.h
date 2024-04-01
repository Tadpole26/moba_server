#pragma once
#include "msg_common.pb.h"
#include "msg_module_servercommon.pb.h"
#include "result_code.pb.h"
#include "msg_module.pb.h"
#include "log_mgr.h"
#include "global_define.h"
#include "msg_parser.h"

using namespace ProtoMsg;

class CRoomThread;
class CBattleUser;
class CRoom;
static const int TICK_MILL_SEC = 10;
static const int MAX_TECV_TIMES = 512;

class CTcpUdpLogic
{
public:
	CTcpUdpLogic();
	virtual ~CTcpUdpLogic();

	virtual bool Init(int iPort, CRoomThread* pRoomNet) = 0;
	virtual void Run() = 0;
	virtual void Stop() = 0;
	virtual void Fini() = 0;
	virtual void OnTimer(int iTime, int64_t llTick) = 0;
	virtual void Tick(int64_t llTick) = 0;

	virtual void KillUser(CBattleUser* pUser) = 0;
	virtual void EnterUser(CBattleUser* pUser) = 0;
	virtual void DeleteUser(CBattleUser* pUser) = 0;
	virtual void AddRoom(CRoom* pRoom) = 0;
	virtual void RemoveRoom(CRoom* pRoom) = 0;
};

class CTcpUdpSession
{
public:
	CTcpUdpSession();
	virtual ~CTcpUdpSession();

	virtual bool Send_Msg(google::protobuf::Message* pMsg, int iCmd
		, int iModule = MsgModule::Battle, ResultCode eCode = Code_Common_Success
		, int iSeqid = 0) = 0;

	virtual bool Send_Msg(const std::string& strMsg, int iCmd
		, int iModule = MsgModule::Battle, ResultCode eCode = Code_Common_Success
		, int iSeqid = 0) = 0;

	virtual bool Send_Msg(const uchar* pBuf, size_t uiSize, int iCmd
		, int iModule = MsgModule::Battle, ResultCode eCode = Code_Common_Success
		, int iSeqid = 0) = 0;

	virtual bool Send(const tagMsgHead* pMsg) = 0;

	virtual bool IsTcp() = 0;								//ÊÇtcp»¹ÊÇudp
	virtual bool InitTcpUdp(int64_t llCreate) = 0;
	virtual void Tick(int64_t llTick) = 0;
	virtual bool IsOut();
	int TickMill(int64_t llVal);
	int AddResetTimes();

	int GeetSequence() const { return m_iSequence; }
	void SetSequence(int iVal) { m_iSequence = iVal; }

	CBattleUser* GetBtlUser() const { return m_pBattleUser; }
	void SetBtlUser(CBattleUser* pUser) { m_pBattleUser = pUser; }

	time_t GetLastTime() const { return m_llLastRecvTime; }
	void SetLastTime(time_t llVal) { m_llLastRecvTime = llVal; }

	int GetConvId() const { return m_iConvId; }
	void SetConvId(int iVal) { m_iConvId = iVal; }

	int64_t GetCreateMs() const { return m_llCreateMs; }

	bool IsStoped() const { return m_bStoped; }
	void SetStoped(bool bVal) { m_bStoped = bVal; }

	int64_t GetUserId() const { return m_llUserId; }
	void SetUserId(int64_t llVal) { m_llUserId = llVal; }

	void AddPing() { ++m_iPingCnt; }
	int GetPingCnt() { return m_iPingCnt; }
protected:
	int				m_iSequence = 0;
	CBattleUser*	m_pBattleUser = nullptr;
	time_t			m_llLastRecvTime = GetCurrTime();
	int				m_iConvId = 0;
	int64_t			m_llCreateMs = 0;
	bool			m_bStoped = false;
	int64_t			m_llUserId = 0;
	int				m_iResetTimes = 0;
	time_t			m_llReset = 0;
	int				m_iPingCnt = 0;
};
