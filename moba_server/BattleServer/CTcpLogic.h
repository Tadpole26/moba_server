#pragma once
#include "CTcpUdpLogic.h"
#include "server_logic_face.h"
#include "logic_thread.h"

class CRoomThread;
class CBattleUser;

class CTcpLogic : public CSvrLogicFace, public CTcpUdpLogic
{
public:
	CTcpLogic();
	~CTcpLogic();

	virtual bool Init(int iPort, CRoomThread* pRoomNet);
	virtual void Run();
	virtual void handle_logic_msg(const tagNetMsg* pNetMsg);
	virtual void KillUser(CBattleUser* pBtlUser);
	virtual void EnterUser(CBattleUser* pBtlUser);
	virtual void DeleteUser(CBattleUser* pBtlUser);
	virtual void AddRoom(CRoom* pRoom);
	virtual void RemoveRoom(CRoom* pRoom);

	virtual void Stop();
	virtual void Fini();
	virtual void OnTimerTcp(int iTime);
	virtual void OnTimer(int iTime, int64_t llTick) {}
	virtual void Tick(int64_t llTick);

private:
	bool my_send_conn_msg(thread_oid_t usToid, conn_oid_t usCoid, const tagMsgHead* pMsg);
	//tcp处理消息
	void EnterRoomTcp(const tagMsgHead* pNetHead, const tagHostHd& stHead);

	CTimeEvent* m_pHeartEvent = nullptr;
	CLogicThread* m_pLogic = nullptr;
	CRoomThread* m_pRoomNet = nullptr;

	std::thread m_thread;
	int m_iPort = 0;
};
