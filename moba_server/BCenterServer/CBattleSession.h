#pragma once
#include "svr_client.h"
namespace ProtoMsg
{
	class Msg_ServerBattle_BC_Register_Req;
}

class CBattleSession : public svr_session
{
public:
	CBattleSession();
	~CBattleSession();

	virtual void Init();
	virtual void Release();

	void Init(ProtoMsg::Msg_ServerBattle_BC_Register_Req& oRegisterReq);

	virtual void handle_msg(const tagMsgHead* pMsg);
	void handle_msg(msg_id_t usCmd, const unsigned char* pMsg, size_t uiSize);

	void on_disconnect();

	void disconnected();
	const std::string& GetBindIp() const { return m_strBindIp; }
	const std::string& GetLinkIp() const { return m_strLinkIp; }

	int GetBindPort() const { return m_iBindPort; }

	int GetMaxRoomNum() const { return m_iMaxRoomNum; }
	void SetMaxRoomNum(int iMax) { m_iMaxRoomNum = iMax; }

	int GetRoomNum() const { return m_iRoomNum; }
	void SetRoomNum(int iRoomNum) { m_iRoomNum = iRoomNum; }

	int GetLeftNum() { return m_iRoomNum >= m_iMaxRoomNum ? 0 : m_iMaxRoomNum - m_iRoomNum; }

	int GetUserNum() const { return m_iUserNum; }
	void SetUserNum(int iVal) { m_iUserNum = iVal; }

	bool IsFull() { return m_iRoomNum >= m_iMaxRoomNum || m_iUserNum >= m_iMaxRoomNum; }

	bool IsValid() { return !IsFull(); }

	int GetServerGroup() const { return m_iServerGroup; }
	void SetServerGroup(int iVal) { m_iServerGroup = iVal; }

	int GetMaxUsernum() const { return m_iMaxUserNum; }
	void SetMaxUserNum(int iVal) { m_iMaxRoomNum = iVal; }

	int GetLeftRoom();
	int GetLeftUser();

	void PushWaitLst();
private:
	std::string m_strBindIp;
	std::string m_strLinkIp;

	int m_iBindPort = 0;
	int m_iMaxRoomNum = 0;
	int m_iRoomNum = 0;
	int m_iUserNum = 0;
	int m_iServerGroup = 0;
	int m_iMaxUserNum = 0;

protected:
	void InitData(const std::string& strBindIp, int iBindPort);

	void OnCreateRoomRsp(const unsigned char* pMsg, size_t uiSize);
	void OnRoomInfoSubmit(const unsigned char* pMsg, size_t uiSize);
	void OnLoadInfoSubmit(CLogicSession* pSession, const unsigned char* pMsg, size_t uiSize);
	void OnBattleResultNotify(const unsigned char* pMsg, size_t uiSize);
};