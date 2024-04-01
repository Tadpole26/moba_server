#pragma once
#include <cstdint>
#include <string>
#include "msg_common.pb.h"
#include "msg_module_battle.pb.h"
#include "msg_module_serverinner.pb.h"
#include "CBattleCommon.h"
#include "msg_make.h"

namespace ProtoMsg
{
	class user_battle_info_t;
	class user_battle_result_t;
	class battle_order_t;
	class Msg_ServerInner_CB_CreateRoom_req;
}

class CBattleUser;
class CRoomThread;
class CRoom : public CMsgMake
{
public:
	CRoom();
	~CRoom();

	void Init();
	void InitRoomData(ProtoMsg::Msg_ServerInner_CB_CreateRoom_req& oCreateReq);
	void Release();

	int GetIndex() const { return m_iIndex; }
	void SetIndex(int iIndex) { m_iIndex = iIndex; }

	int64_t GetRoomId() const { return m_llRoomId; }
	void SetRoomId(int64_t llRoomId) { m_llRoomId = llRoomId; }

	const std::string& GetLogId() const { return m_strLogId; }
	void SetLogId(const std::string& strLogId) { m_strLogId = strLogId; }

	bool IsUdp() const { return m_bUdp; }
	void SetUdp(bool val) { m_bUdp = val; }

	int GetHostPort() const { return m_iPort; }
	void SetHostPort(int val) { m_iPort = val; }

	int GetUserNum() const { return m_iUserNum; }
	int GetInUserNum() const { return (int)m_setUserIn.size(); }
	int GetUserDataNum() const { return (int)m_vecUsersData.size(); }
	int GetObUserDataNum() const { return (int)m_vecObUsersData.size(); }

	ProtoMsg::ERoomStatus GetRoomStatus() const { return m_eRoomStatus; }

	bool IsRun() { return m_eRoomStatus == ProtoMsg::ERoomStatus_Run; }
	bool IsEnd() { return  m_eRoomStatus == ProtoMsg::ERoomStatus_End; }
	int GetAllFrame() { return m_iFrameCount; }

	CRoomThread* GetRoomNet() const { return m_pRoomNet; }
	void SetRoomNet(CRoomThread* val) { m_pRoomNet = val; }

	std::map<int64_t, int>& GetConvList() { return m_mapUserConvId; }

	ProtoMsg::user_battle_info_t* GetUserData(std::string strSessionId, bool bAll = false);
	ProtoMsg::user_battle_info_t* GetUserDataById(int64_t llUid, bool bAll = false);

	//ս����ʼ
	void OnBattleStart(int64_t llTick, bool bFirst = true);
	//��ҽ��뷿��
	void Enter(CBattleUser* pBtlUser);
	//����뿪����
	void Leave(CBattleUser* pBtlUser);
	void KillUser(CBattleUser* pBtlUser, int64_t llTick);
	void DeleteUser(CBattleUser* pBtlUser);
	void KickAllUser(int64_t llTick);
	//������Ƿ�ս���������ϴ�ս��
	bool IsOver(size_t uiPos) { return !m_vecUserResult[uiPos].empty(); }
	//������Ҳ���ָ��(�뿪����ȵ�)
	void AddOrder(const ProtoMsg::battle_order_t& stData, CBattleUser* pBtlUser = nullptr);
	//ͳ�Ʒ����Ѿ�׼�����
	int ReadySize();
	//ͳ�Ʒ����Ѿ�׼���õĹ�ս���
	int ReadyObSize();
	//��������Ƿ���ָ�������ս������(ս���������ϴ�ս��)
	bool IsAllOver();
	//�������idƴ�ӳ�Ψһ�ַ���
	const std::string& MakeIdList();
	//�ͻ������󷿼����ݻظ�
	void SendEnterRoom(CBattleUser* pBtlUser);
	//
	void SendLoadProgress(CBattleUser* pBtlUser);
	//�㲥�����������������
	void SendAll(int iMsgId, ProtoMsg::ResultCode eCode, google::protobuf::Message* pMsg, bool bAll = true);
	//�Ƿ�ʱ
	bool IsTimeOut(int64_t llTick);

	//֡ͬ�����ݸ��ͻ���
	void OnDoHeart(int64_t llTick);
	void OnHeartbeat(int64_t llTick);
	int OnTimer(int64_t llTick);

	std::vector<ProtoMsg::user_battle_result_t>& GetBattleResult() { return m_vecResult; }
	ProtoMsg::user_battle_result_t* GetResult(int64_t llUid);
	void AddResult(int iPos, std::vector<ProtoMsg::user_battle_result_t>& vecRes, std::string& strResult, int64_t llUid);
	void AddKeyType(size_t uiPos, int ikeyLst);
	size_t GetResultPos(std::string strResult);
	void CheckResult();
	//�ϴ�ս���Ƿ�ȫ��ƽ��
	bool CheckResultPvg();		
	//�ϴ�ս���Ƿ�һ��ʤ��
	bool CheckResultWin();
	//�ϴ�ս���Ƿ�һ��
	bool CheckResultPerson();
	void MakeEndResult(int64_t llTick);

	void DoEnd();
	void PushEnd(int64_t llTick);

private:
	CRoomThread* m_pRoomNet = nullptr;

	bool m_bUdp = false;
	int m_iIndex = -1;
	int m_iPort = 0;
	int m_iRandSeed = 0;										//���������
	ProtoMsg::ERoomType m_eRoomType = ProtoMsg::ERoomType_None;				//��������
	int64_t m_llRoomId = 0;											//����id
	ProtoMsg::ERoomStatus m_eRoomStatus = ProtoMsg::ERoomStatus_None;		//����״̬
	battle_map_info_t m_stMapInfo;									//��ͼ��Ϣ
	std::string m_strLogId;
	int m_iUserNum = 0;													//����������
	ProtoMsg::ELeisureType m_eLeisureType = ProtoMsg::ELeisureType_None;			//����ģʽ����

	int64_t m_llCreateTime = 0;							//��������ʱ��
	int64_t m_llBeginTime = 0;							//ս����ʼʱ��
	int64_t m_llEndTime = 0;							//ս������ʱ��
	int64_t m_llLastLeaveTime = 0;						//���һ�����뿪��ʱ���

	std::vector<ProtoMsg::user_battle_info_t> m_vecUsersData;		//�ڷ����������(�����������,һֱ����)
	std::vector<ProtoMsg::user_battle_info_t> m_vecObUsersData;		//�ڷ����ս�������(�����������,һֱ����)
	std::string m_strUserList;							//�ڷ������useridƴ�ӵ��ַ���
	std::set<CBattleUser*> m_setUserIn;					//�ڷ���ս�����(socket���ӵ����,����ʲô��������)
	std::set<CBattleUser*> m_setObUserIn;				//�ڷ����ս���(socket���ӵ����,����ʲô��������)
	std::map<int64_t, time_t> m_mapUserOff;				//����������
	std::map<int64_t, time_t> m_mapHasEnter;			//����������Ϸ���
	std::set<int64_t> m_setEscape;						//�������
	std::vector<int64_t> m_vecRobotsIn;					//������
	std::map<int64_t, int>	m_mapUserConvId;			//���udp��convid����

	/////////////////////////////////////////////////////////////////////////////////////
	int64_t m_llPreFrameTime = 0;													//��һ֡�㲥��ʱ��
	int m_iEmptyFrame = 0;															//��֡
	int64_t m_llMaxFrameMs = 0;
	int64_t m_llMinFrameMs = 0;
	int		m_iFrameCount = 0;														//�ܹ�������֡��
	int		m_iNetFrame = 0;														//��һ��ִ֡�е���׼ȷʱ���
	::google::protobuf::RepeatedPtrField<ProtoMsg::battle_order_t> m_vecCurOrders;	//��ǰ֡��������	
	std::vector<ProtoMsg::battle_order_t>	m_vecRecOrders;							//��������֡��������
	std::vector<int> m_vecRecpos;													//ÿ֡����λ��
	////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////
	ProtoMsg::EBRCode m_eResFail = ProtoMsg::EBRCode_Success;						//ս��ս������
	//��ȷ���
	std::vector<ProtoMsg::user_battle_result_t> m_vecResult;	//�洢����ϱ���ս��
	std::string m_strResult;
	std::vector<int> m_vecKeyType;

	//����ϱ��Ľ��
	std::vector<std::string> m_vecStrResult;
	std::map<std::string, int> m_mapResultTimes;	//����ϴ����������ս�����л���string��Ӧ�ϴ�����
	std::vector<std::vector<ProtoMsg::user_battle_result_t>> m_vecUserResult;	//λ�ö�Ӧ�ϴ��Ķ��ս��(ÿ���˶����ϴ�������ҵ�ս��)
	std::set<int64_t> m_setUserOver;
	int m_iMustResNum = 0;
	int m_iHaveOverNum = 0;											//�ϱ��������
	time_t m_llOverBegin = 0;										//�Ƿ��Ѿ�������ϱ����

	ProtoMsg::Msg_RoomHeartbeat_Notify m_oCliBeatNorify;
	ProtoMsg::Msg_Battle_Result_Notify m_oCliResultNotify;
	ProtoMsg::Msg_ServerInner_BC_Result_Notify m_oSvrResultNotify;
	////////////////////////////////////////////////////////////////////////////////////
};