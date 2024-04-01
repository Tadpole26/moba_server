#pragma once
#include "svr_client.h"
#include "msg_common.pb.h"
#include "CTcpSess.h"
#include "CUdpSess.h"
#include "msg_module_battle.pb.h"

class CRoom;
class CBattleUser : public svr_session
{
public:
	CBattleUser();
	~CBattleUser();

	void Init();
	void Release();
	void Kill(int64_t llTick = 0);

	void handle_msg(const tagMsgHead* pHead);
	bool IsTimeOut();
	virtual void on_disconnect();

	//��ʼ��tcp����session
	CTcpSess* InitTcp();
	CUdpSess* InitUdp();

	CTcpSess* GetTcpSess() const { return dynamic_cast<CTcpSess*>(m_pTcpUdpSess); }
	CUdpSess* GetUdpSess() const { return dynamic_cast<CUdpSess*>(m_pTcpUdpSess); }

	int GetSeqId() { return m_iSequenceId; }

	//������ҷ���
	void SetRoom(CRoom* pRoom);
	CRoom* GetRoom() { return m_pRoom; }

	//������ҷ���id
	int64_t GetRoomId() { return m_llRoomId; }

	bool IsLoadEnd() { return m_bLoadEnd; }
	void SetLoadEnd(bool bLoadEnd) { m_bLoadEnd = bLoadEnd; }

	bool IsReady() { return m_bReady; }
	void SetReady(bool val) { m_bReady = val; }

	bool IsOver() { return m_bOver; }
	void SetOver(bool bOver) { m_bOver = bOver; }

	bool IsInit() { return m_bInit; }

	int GetConvId() const { return m_iConvId; }
	void SetConvId(int iVal) { m_iConvId = iVal; }

	int GetPosition() const { return m_iPosition; }

	int GetLoadProgress() const { return m_iLoadProgress; }
	void SetLoadProgress(int iVal) { m_iLoadProgress = iVal; }

	void SetUserBaseData(ProtoMsg::user_battle_info_t& oUserData);
	int64_t GetUserId() { return m_llUid; }
	const std::string& GetUserName() { return m_strUserName; }
	bool IsOb() { return m_bOb; }

	void InitIp();
	std::string GetIp();

	void SendProtol(google::protobuf::Message* pMsg, msg_id_t usCmd, int iCode, int iSequence);
	//�ͻ�������ս����������
	void OnEnterRoom(ProtoMsg::Msg_Battle_EnterRoom_Req& oEnterReq, int iSequence);

	bool IsBeginKill() const { return m_bBeginKill; }
	void SetBeginKill(bool bVal) { m_bBeginKill = bVal; }

protected:
	void OnMsgEnterRoomReq(const tagMsgHead* pHead);
private:
	int m_iSequenceId = 0;

	int64_t m_llRoomId = 0;							//��ǰ������ڷ���id
	CRoom* m_pRoom = nullptr;						//��ǰ������ڵķ���

	bool m_bInit = false;
	bool m_bLoadEnd = false;
	bool m_bBeginKill = false;
	bool m_bReady = false;
	bool m_bOver = false;

	std::vector<ProtoMsg::user_battle_result_t> m_vecResult;
	std::string m_strResult;
	Msg_Battle_OverRoom_Req m_oOverMsg;

	int m_iLoadProgress = 0;
	uint32_t m_uiLittleOrderMs = 0;	
	uint32_t m_uiBigOrderMs = 0;

	int64_t m_llUid = 0;							//���Ψһid
	std::string m_strUserName;						//����ǳ�
	std::string m_strSessionid;						//����ַ�������

	bool m_bTcp = false;							//�Ƿ���tcp����
	std::string m_strIp;
	CTcpSess m_oTcpSess;							//tcp������Ϣ
	CUdpSess m_oUdpSess;							//udp������Ϣ
	CTcpUdpSession* m_pTcpUdpSess = nullptr;	

	int m_iPosition = 0;							//ƥ���е�λ��
	int m_iConvId = 0;
	bool m_bOb = false;								//�Ƿ��ǹ�ս���

};
