#include "CRoom.h"
#include "CBattleUser.h"
#include "msg_common.pb.h"
#include "msg_module_battle.pb.h"
#include "msg_module_serverinner.pb.h"
#include "CRoomThread.h"
#include "CRoomManager.h"
#include "util_strop.h"
#include "parse_pb.h"
#include "CBattleLogic.h"

const int MAX_BEGIN_WAIT = 150000;
//�����Ա,���ٿ�����ʱ��
CRoom::CRoom()
{
}

CRoom::~CRoom()
{
	Release();
}

void CRoom::Init()
{
	m_iIndex = -1;
	m_llRoomId = 0;

	m_iNetFrame = 0;							//��һ��ִ֡�е���׼ȷʱ���
	m_llPreFrameTime = 0;						//��һ��֡ͬ��ִ�е�ʱ���
	m_llBeginTime = 0;							//ս����ʼ��ʱ���
	m_iFrameCount = 0;							//�ܹ�������֡��
	m_llEndTime = 0;							//ս��������ʱ���
	m_eRoomType = ERoomType_None;				//��������
	m_eRoomStatus = ERoomStatus_None;			//����״̬
	m_eResFail = EBRCode_Success;				//ս��ս������
	m_iHaveOverNum = 0;							//�ϱ��������
	m_llOverBegin = 0;							//�Ƿ��Ѿ�������ϱ����
	m_llLastLeaveTime = 0;						//���һ�����뿪��ʱ���
	//m_bSyncCrc = true;							//ͬ��У��
	//m_bAutoStart = false;
	//m_bAutoBattle = false;
	m_iEmptyFrame = 0;
	m_llMaxFrameMs = 0;
	m_llMinFrameMs = 0;
	m_iUserNum = 0;
}

void CRoom::Release()
{
	m_strLogId.clear();
	m_stMapInfo.Clear();						//��ͼ��Ϣ
	m_vecUsersData.clear();						//�ڷ����������(�����������,һֱ����)
	m_vecObUsersData.clear();					//�ڷ����ս�������(�����������,һֱ����)
	m_vecCurOrders.Clear();						//��ǰ֡��������	
	m_vecRecOrders.clear();						//��������֡��������
	m_vecRecpos.clear();						//ÿ֡����λ��
	m_setUserIn.clear();						//�ڷ���ս�����(socket���ӵ����,����ʲô��������)
	m_setObUserIn.clear();						//�ڷ����ս���(socket���ӵ����,����ʲô��������)
	m_vecRobotsIn.clear();						//������
	m_vecResult.clear();
	m_vecStrResult.clear();
	m_mapResultTimes.clear();
	m_vecUserResult.clear();					//���ƥ��λ�ö�Ӧ��ս������
	//m_mapSyncrc.clear();
	m_mapUserOff.clear();						//����������
	m_mapUserConvId.clear();					//���udp��convid����
	m_mapHasEnter.clear();						//����������Ϸ���
	m_strUserList.clear();
}

int CRoom::ReadySize()
{
	int iRetNum = 0;
	
	for (auto& iterUser : m_setUserIn)
	{
		if (iterUser->IsReady())
			++iRetNum;
	}
	return iRetNum;
}

int CRoom::ReadyObSize()
{
	int iRetNum = 0;

	for (auto& iterUser : m_setObUserIn)
	{
		if (iterUser->IsReady())
			++iRetNum;
	}
	return iRetNum;
}

bool CRoom::IsAllOver()
{
	if (!m_iHaveOverNum) return false;

	for (auto& user : m_setUserIn)
	{
		if (!user->IsLoadEnd() || !user->IsOver())
			return false;
	}
	return true;
}

const std::string& CRoom::MakeIdList()
{
	if (!m_strUserList.empty())
		return m_strUserList;

	std::vector<int64_t> vecUserId;
	for (size_t i = 0; i < m_vecUsersData.size(); ++i)
		vecUserId.push_back(m_vecUsersData[i].ouserinfo().lluid());
	m_strUserList = MakeCVStr(vecUserId);
	return m_strUserList;
}

void CRoom::InitRoomData(ProtoMsg::Msg_ServerInner_CB_CreateRoom_req& oCreateReq)
{
	m_vecUserResult.resize(oCreateReq.vecuserdata_size());
	//vecKeyType.resize(oCreateReq.vecuserdata_size());
	m_vecStrResult.resize(oCreateReq.vecuserdata_size());

	m_llRoomId = oCreateReq.llroomid();
	m_eRoomType = oCreateReq.etype();
	m_iRandSeed = (int)GetCurrTime();
	m_eRoomStatus = ERoomStatus_Start;
	m_llCreateTime = GetMillSec();					//����
	m_eLeisureType = oCreateReq.eleisuretype();
	m_stMapInfo.m_iMapId = oCreateReq.imapid();
	m_stMapInfo.m_strMapName = oCreateReq.strmapname();
	m_stMapInfo.m_iSecond = oCreateReq.imapsec();
	m_vecUsersData.resize(oCreateReq.vecuserdata_size());
	m_vecObUsersData.resize(oCreateReq.vecobuserdata_size());

	//�����Ϣ
	for (int i = 0; i < oCreateReq.vecuserdata_size(); ++i)
	{
		const ProtoMsg::user_battle_info_t& stUser = oCreateReq.vecuserdata(i);
		ProtoMsg::user_battle_info_t& stSvrData = m_vecUsersData[i];
		stSvrData.CopyFrom(stUser);

		if (stUser.ouserinfo().erobottype() != ERobotType::ERobotType_None)
			m_vecRobotsIn.push_back(stUser.ouserinfo().lluid());
		else
		{
			m_iUserNum++;
			int iConvId = gRoomManager->RandConv();
			stSvrData.set_iconvid(iConvId);
			m_mapUserConvId[stUser.ouserinfo().lluid()] = iConvId;
		}
	}
	//��ս����Ϣ
	for (int i = 0; i < oCreateReq.vecobuserdata_size(); ++i)
	{
		const ProtoMsg::user_battle_info_t& stObUser = oCreateReq.vecuserdata(i);
		ProtoMsg::user_battle_info_t& stSvrObData = m_vecObUsersData[i];
		stSvrObData.CopyFrom(stObUser);
		stSvrObData.set_bob(true);
		int iConvId = gRoomManager->RandConv();
		stSvrObData.set_iconvid(iConvId);
		m_mapUserConvId[stObUser.ouserinfo().lluid()] = iConvId;
	}

	m_vecCurOrders.Reserve((int)m_vecUsersData.size());
	m_vecRecpos.reserve(20*300);
	m_vecRecOrders.reserve(3000);
	m_vecUserResult.reserve(m_vecUsersData.size());
	m_vecStrResult.reserve(m_vecUsersData.size());
	//vecKeyType.reserve(m_vecUsersData.size());
	m_vecResult.reserve(m_vecUsersData.size());
	m_iMustResNum = (int)m_vecUsersData.size();

}

void CRoom::Enter(CBattleUser* pBtlUser)
{
	if (m_pRoomNet != nullptr)
		m_pRoomNet->EnterUser(pBtlUser);

	if (pBtlUser->IsOb())
		m_setObUserIn.insert(pBtlUser);
	else
	{
		m_setUserIn.insert(pBtlUser);
		m_mapHasEnter[pBtlUser->GetUserId()] = GetCurrTime();

		time_t llLeaveTime = m_llBeginTime / 1000;
		//��������
		auto itLeave = m_mapUserOff.find(pBtlUser->GetUserId());
		if (itLeave != m_mapUserOff.end())
		{
			llLeaveTime = itLeave->second;
			m_mapUserOff.erase(itLeave);
		}

		//��Ϸ��ʼ���ҵ���̫����Ϊ����(����ʮ������Ϊ����)
		if (llLeaveTime > 0 && (GetCurrTime() - llLeaveTime) >= 600)
			m_setEscape.insert(pBtlUser->GetUserId());
	}
}

void CRoom::Leave(CBattleUser* pBtlUser)
{
	m_llLastLeaveTime = GetCurrTime();				//����뿪��ҵ�ʱ���
	if (!pBtlUser->IsOb())
	{
		m_mapUserOff[pBtlUser->GetUserId()] = GetCurrTime();
		m_setUserIn.erase(pBtlUser);
	}
	else
		m_setObUserIn.erase(pBtlUser);

	if (m_eRoomStatus == ProtoMsg::ERoomStatus_Run)
	{
		ProtoMsg::battle_order_t stData;
		stData.set_iexectime(m_iNetFrame);
		stData.set_iuserid(pBtlUser->GetPosition());
		stData.set_itype(254);
		//����뿪ʱ���
		AddOrder(stData);
	}

	//��������
	if (m_pRoomNet)
		m_pRoomNet->DeleteUser(pBtlUser);
}

void CRoom::KillUser(CBattleUser* pBtlUser, int64_t llTick)
{
	if (m_pRoomNet != nullptr)
		m_pRoomNet->KillUser(pBtlUser, llTick);
}

void CRoom::DeleteUser(CBattleUser* pBtlUser)
{
	if (m_pRoomNet != nullptr)
		m_pRoomNet->DeleteUser(pBtlUser);
}

void CRoom::KickAllUser(int64_t llTick)
{
	//����
	std::set<CBattleUser*> setUser;
	setUser.swap(m_setUserIn);
	for (auto& user : setUser)
		user->Kill(llTick);
	setUser.clear();
	setUser.swap(m_setObUserIn);
	for (auto& user : setUser)
		user->Kill(llTick);
}

void CRoom::OnBattleStart(int64_t llTick, bool bFirst)
{
	m_iNetFrame = NET_FRAME_TIME;
	m_eRoomStatus = ProtoMsg::ERoomStatus_Run;

	//if (bFirst)
	//	CNetFaceProxy::GetInstance()->SubmitRoomInfo(this);

	m_iFrameCount = 0;
	m_llBeginTime = llTick; //����
	m_llPreFrameTime = m_llBeginTime; 

	Msg_RoomStart_Notify oNotify;
	oNotify.set_iroomtime(m_iNetFrame);
	oNotify.set_iservertime((int)(llTick - m_llCreateTime));		//�������䵽��ʼ��Ϸ��ʱ��(����)
	SendAll(MsgModule_Battle::Msg_RoomStart_Notify, ResultCode::Code_Common_Success, &oNotify);
}

void CRoom::AddOrder(const ProtoMsg::battle_order_t& stData, CBattleUser* pBtlUser)
{
	if (m_eRoomStatus == ERoomStatus_Run)
	{
		battle_order_t* pData = m_vecCurOrders.Add();
		if (pData)
		{
			pData->CopyFrom(stData);
			pData->set_iexectime(m_iNetFrame);
		}
	}
}

ProtoMsg::user_battle_info_t* CRoom::GetUserData(std::string strSessionId, bool bAll)
{
	for (size_t i = 0; i < m_vecUsersData.size(); ++i)
	{
		if (m_vecUsersData[i].ouserinfo().erobottype() > ERobotType_None)
			continue;

		if (strSessionId.compare(m_vecUsersData[i].strusersessionid()) == 0)
			return &m_vecUsersData[i];
	}

	if (!bAll) return nullptr;

	for (size_t i = 0; i < m_vecObUsersData.size(); ++i)
	{
		if (m_vecObUsersData[i].ouserinfo().erobottype() > ERobotType_None)
			continue;

		if (strSessionId.compare(m_vecObUsersData[i].strusersessionid()) == 0)
			return &m_vecObUsersData[i];
	}
	return nullptr;
}

ProtoMsg::user_battle_info_t* CRoom::GetUserDataById(int64_t llUid, bool bAll)
{
	for (auto& data : m_vecUsersData)
	{
		if (data.ouserinfo().lluid() == llUid)
			return &data;
	}

	if (!bAll) return nullptr;

	for (auto& data : m_vecObUsersData)
	{
		if (data.ouserinfo().lluid() == llUid)
			return &data;
	}
	return nullptr;
}

void CRoom::SendEnterRoom(CBattleUser* pBtlUser)
{
	Msg_Battle_EnterRoom_Rsp oEnterRsp;
	oEnterRsp.set_strmapname(m_stMapInfo.m_strMapName);
	oEnterRsp.set_etype(m_eRoomType);
	oEnterRsp.set_irandseed(m_iRandSeed);
	oEnterRsp.set_estatus(m_eRoomStatus);
	oEnterRsp.set_iroomtime(m_iNetFrame);
	if (m_eRoomType == ERoomStatus_Run)
		oEnterRsp.set_istarttime(int(m_llBeginTime - m_llCreateTime));
	else
		oEnterRsp.set_istarttime(0);
	pBtlUser->SendProtol(&oEnterRsp, MsgModule_Battle::Msg_Battle_EnterRoom_Rsp
	, ResultCode::Code_Common_Success, pBtlUser->GetSeqId());
}

void CRoom::SendLoadProgress(CBattleUser* pBtlUser)
{
	Msg_RoomLoadProgress_Notify oPush;
	for (auto& itr : m_setUserIn)
	{
		ProtoMsg::load_progress_info_t* pProgressInfo = oPush.add_vecload();
		if (pProgressInfo != nullptr)
		{
			pProgressInfo->set_lluserid(itr->GetUserId());
			pProgressInfo->set_iprogress(itr->GetLoadProgress());
		}
	}

	//��ս��
	for (auto& itr : m_setObUserIn)
	{
		ProtoMsg::load_progress_info_t* pProgressInfo = oPush.add_vecload();
		if (pProgressInfo != nullptr)
		{
			pProgressInfo->set_lluserid(itr->GetRoomId());
			pProgressInfo->set_iprogress(itr->GetLoadProgress());
		}
	}

	pBtlUser->SendProtol(&oPush, MsgModule_Battle::Msg_RoomLoadProgress_Notify, ResultCode::Code_Common_Success, 0);
}

void CRoom::SendAll(int iMsgId, ProtoMsg::ResultCode eCode, google::protobuf::Message* pMsg, bool bAll)
{
	tagMsgHead* pNetMsgHead = MakeHeadMsg(pMsg, ProtoMsg::Battle, (uint32_t)iMsgId, 0, (ProtoMsg::ResultCode)eCode);
	if (pNetMsgHead == nullptr)
	{
		Log_Error("pNetMsgHead is nullptr");
		return;
	}

	for (auto& iter : m_setUserIn)
	{
		if (!iter->IsBeginKill() && iter->IsInit() && iter->GetRoomId() == m_llRoomId)
			iter->Send(pNetMsgHead);
	}

	if (!bAll)
		return;

	//Ĭ�ϸ����й�ս�߷�����Ϣ
	for (auto& iter : m_setObUserIn)
	{
		if (!iter->IsBeginKill() && iter->IsInit() && iter->GetRoomId() == m_llRoomId)
			iter->Send(pNetMsgHead);
	}
}

void CRoom::DoEnd()
{
	std::string strSend;
	if (!SerToString(m_oSvrResultNotify, strSend))
		return;

	//���ͽ���֪ͨ,����ֻ�������߳��е���,���Բ��ö��̴߳���
	//CNetFaceProxy::GetInstance()->SendWorldMsg(strSend, MsgModule_ServerBattle::);
}

void CRoom::PushEnd(int64_t llTick)
{
	m_eRoomStatus = ProtoMsg::ERoomStatus_End;
	m_llEndTime = llTick;

	//���ս�����
	CheckResult();
	//֪ͨ�ͻ��˽��㿪ʼ
	MakeEndResult(llTick);
	//�߳����з������
	KickAllUser(llTick);
	//ѹ����㴦��
	if (m_pRoomNet != nullptr)
		m_pRoomNet->PushDestroy(this, llTick);

	if (m_eResFail != EBRCode_Empty && m_eResFail != EBRCode_Success)
	{
		Log_Error("roomid:%lld, logid:%s, type:%d, resultcode:%d, user:%s",
			m_llRoomId, m_strLogId.c_str(), m_eRoomType, m_eResFail, MakeIdList().c_str());
	}
	Log_Custom("info", "roomid:%lld, logid:%s, type:%d, resultcode:%d, user:%s",
		m_llRoomId, m_strLogId.c_str(), m_eRoomType, m_eResFail, MakeIdList().c_str());
}

bool CRoom::IsTimeOut(int64_t llTick)
{
	if (m_eResFail == EBRCode_NoStart || m_eResFail == EBRCode_Timeout ||
		m_eResFail == EBRCode_ResTimeout || m_eResFail == EBRCode_OfflineTimeout)
		return true;

	if (m_eRoomStatus != ERoomStatus_Start &&
		(llTick - m_llBeginTime) > (m_stMapInfo.m_iSecond + 10) * 1000)
	{
		m_eResFail = EBRCode_Timeout;
		return true;
	}

	if (m_eRoomStatus == ERoomStatus_Start && m_setUserIn.size() == 0 && (llTick - m_llCreateTime) >= MAX_BEGIN_WAIT)
	{
		m_eResFail = EBRCode_NoStart;
		return true;
	}

	//�����ϱ����,�ȴ��������ϱ����
	if (m_llOverBegin != 0 && (GetCurrTime() - m_llOverBegin) >= MAX_BEGIN_WAIT)
		return true;

	return false;
}

/////////////////////////////////////////////֡ͬ������////////////////////////////////////////////////////
void CRoom::OnDoHeart(int64_t llTick)
{
	++m_iFrameCount;
	m_iNetFrame = m_iFrameCount * NET_FRAME_TIME + NET_FRAME_TIME;
	m_vecRecpos.push_back((int)m_vecRecOrders.size());

	m_oCliBeatNorify.Clear();
	m_oCliBeatNorify.set_iroomtime(m_iNetFrame);
	m_oCliBeatNorify.set_iservertime((int)(llTick-m_llCreateTime));

	if (m_vecCurOrders.size() > 0)
	{
		m_oCliBeatNorify.mutable_vecorders()->Reserve(m_vecCurOrders.size());

		for (auto& order : m_vecCurOrders)
			m_vecRecOrders.emplace_back(order);

		m_oCliBeatNorify.mutable_vecorders()->Swap(&m_vecCurOrders);
	}
	else
		++m_iEmptyFrame;

	SendAll(MsgModule_Battle::Msg_RoomHeartbeat_Notify, ResultCode::Code_Common_Success, &m_oCliBeatNorify);
}

void CRoom::OnHeartbeat(int64_t llTick)
{
	int64_t llPreFrame = llTick - m_llPreFrameTime;
	if (m_llOverBegin == 0 && m_llPreFrameTime != NET_FRAME_TIME)
	{
		if ((m_llPreFrameTime - 25) > NET_FRAME_TIME)
			++m_llMaxFrameMs;
		else if ((m_llPreFrameTime + 25) < NET_FRAME_TIME)
			++m_llMinFrameMs;
	}

	OnDoHeart(llTick);
	m_llPreFrameTime = llTick;
}

int CRoom::OnTimer(int64_t llTick)
{
	if (m_eRoomStatus == ERoomStatus_None)
		return 0;

	if (m_eRoomStatus == ERoomStatus_End)
		return 1;

	if (IsTimeOut(llTick))	//��ʱ����
	{
		PushEnd(llTick);
		return 1;
	}

	if (m_eRoomStatus == ERoomStatus_Start)
	{
		if (m_setUserIn.size() == 0)
			return 0;

		//ȫ��׼���û򳬹�׼��ʱ��,��ʼս��
		int64_t llCreateMs = llTick - m_llCreateTime;
		int iReadySize = ReadySize();
		int iObReadySize = ReadyObSize();
		if (iReadySize > 0 && ((iReadySize == m_iUserNum && iObReadySize == (int)m_setObUserIn.size())
			|| llCreateMs >= MAX_BEGIN_WAIT))
		{
			OnBattleStart(llTick);
			return 0;
		}

		//û��׼��
		if (llCreateMs >= MAX_BEGIN_WAIT)
		{
			m_eResFail = EBRCode_NoStart;
			PushEnd(llTick);
			return 1;
		}

		return 0;
	}

	//�����ϱ�����
	if (IsAllOver())
	{
		m_vecCurOrders.Clear();
		PushEnd(llTick);
		return 1;
	}

	OnHeartbeat(llTick);

	return 0;
}

//////////////////////////////////////////////ս������/////////////////////////////////////////////////////
ProtoMsg::user_battle_result_t* CRoom::GetResult(int64_t llUid)
{
	for (size_t i = 0; i < m_vecResult.size(); ++i)
	{
		if (m_vecResult[i].lluid() == llUid)
			return &m_vecResult[i];
	}
	return nullptr;
}

void CRoom::AddResult(int iPos, std::vector<ProtoMsg::user_battle_result_t>& vecRes, std::string& strResult, int64_t llUid)
{
	if (llUid == 0)
		return;
	m_setUserOver.insert(llUid);
	if (m_setUserOver.size() == 1)
	{
		ProtoMsg::battle_order_t stOrder;
		stOrder.set_iexectime(m_iNetFrame);
		stOrder.set_iuserid(iPos);
		stOrder.set_itype(255);
		//��һ������ϱ�ս�����,֪ͨ��������ϱ�
		AddOrder(stOrder);
	}

	if (iPos >= m_vecUsersData.size())
		return;

	++m_iHaveOverNum;
	if (m_llOverBegin == 0)
		m_llOverBegin = GetCurrTime();

	m_vecUserResult[iPos] = vecRes;
	m_vecStrResult[iPos] = strResult;
	m_mapResultTimes[strResult]++;
}

void CRoom::AddKeyType(size_t uiPos, int ikeyLst)
{
	if (uiPos >= m_vecUsersData.size())
		return;
	m_vecKeyType[uiPos] = ikeyLst;
}

size_t CRoom::GetResultPos(std::string strResult)
{
	size_t uiPos = 0;
	for (; uiPos < m_vecStrResult.size(); ++uiPos)
	{
		if (m_vecStrResult[uiPos] == strResult)
			break;
	}
	return uiPos;
}

void CRoom::CheckResult()
{
	if (m_eResFail == EBRCode_NoStart)
	{
		m_vecResult.clear();
	}
	else if (m_eResFail == EBRCode_Success)
	{
		if (m_iUserNum >= 4)
			CheckResultPerson();

		if (m_vecResult.size() == 0)
		{
			m_eResFail = EBRCode_Empty;		//������,�����ϱ�
			m_vecResult.clear();
		}
		else if (!CheckResultPvg())
		{
			m_eResFail = EBRCode_PvgDiff;	//ƽ����Ч
			m_vecResult.clear();
		}
		else if (!CheckResultWin())
		{
			m_eResFail = EEBRCode_WinSame;	//ȫʤ��Ч
			m_vecResult.clear();
		}
	}

	//�յ�ս�����,������Ч��ս��,ȫʧ��
	if (m_vecResult.empty())
	{
		m_vecResult.clear();
		m_vecResult.resize(m_vecUsersData.size());
		for (size_t i = 0; i < m_vecUsersData.size(); ++i)
		{
			m_vecResult[i].set_lluid(m_vecUsersData[i].ouserinfo().lluid());
			m_vecResult[i].set_iposition(m_vecUsersData[i].ouserinfo().ipos());
			m_vecResult[i].set_eresult(EBattleResult::EBattleResult_Lose);
			m_vecResult[i].set_llendtime(m_llEndTime);
		}
	}
}

bool CRoom::CheckResultPvg()
{
	bool bHave = false;
	for (const auto& iter : m_vecResult)
	{
		if (iter.eresult() == EBattleResult_DagFall)
		{
			bHave = true;
			break;
		}
	}

	if (bHave)
	{
		for (const auto& iter : m_vecResult)
		{
			if (iter.eresult() != EBattleResult_DagFall)
				return false;
		}
	}

	return true;
}

bool CRoom::CheckResultWin()
{
	std::set<int> setSide;
	//�ϱ��Ľ���к���δ�ڶ����е���,ֱ�ӱ��ʧ��
	//�ϱ�����������Ӫ����ʤ��,Ҳ���Ϊʧ��
	ProtoMsg::user_battle_info_t* pUserData = nullptr;
	for (const auto& iter : m_vecResult)
	{
		if (iter.eresult() == EBattleResult_Win)
		{
			pUserData = GetUserDataById(iter.lluid());
			if (pUserData == nullptr)
				return false;
			else
				setSide.insert(pUserData->ouserinfo().iside());
		}
	}

	if (setSide.size() > 1)
		return false;

	return true;
}

bool CRoom::CheckResultPerson()
{
	if (m_mapResultTimes.size() == 0)
	{
		m_eResFail = EBRCode_Empty;
		return true;
	}
	else if (m_mapResultTimes.size() == 1 && m_vecUserResult.size() > 0)
	{
		//ֻ��һ������,�����ϱ�ս��һ��,��ȷ
		size_t uiPos = GetResultPos(m_mapResultTimes.begin()->first);
		m_vecResult = m_vecUserResult[uiPos];
		return true;
	}
	else if (m_mapResultTimes.size() > 1)
	{
		//����ϴ�ս����ͬ,����
		if (m_mapUserOff.size() > 0)
			m_eResFail = EBRCode_OfflineDiff;
		else
			m_eResFail = EBRCode_Diff;
	}
	return true;
}

void CRoom::MakeEndResult(int64_t llTick)
{
	m_oCliResultNotify.Clear();
	m_oCliResultNotify.mutable_vecdata()->Reserve(m_iUserNum);

	//ս������
	m_oSvrResultNotify.Clear();
	m_oSvrResultNotify.mutable_vecresult()->Reserve(m_iUserNum);

	m_oSvrResultNotify.set_llroomid(m_llRoomId);
	m_oSvrResultNotify.set_strroomlogid(m_strLogId);

	m_oSvrResultNotify.mutable_oroomdata()->set_estatus(m_eRoomStatus);
	m_oSvrResultNotify.mutable_oroomdata()->set_eresfail(m_eResFail);
	m_oSvrResultNotify.mutable_oroomdata()->set_ibattlesec((int)m_iFrameCount/20);
	m_oSvrResultNotify.mutable_oroomdata()->set_bnofullres((size_t)m_iUserNum != m_setUserIn.size());
	m_oSvrResultNotify.mutable_oroomdata()->set_iordersize((int)m_vecRecOrders.size());
	m_oSvrResultNotify.mutable_oroomdata()->set_llmaxframes(m_llMaxFrameMs);
	m_oSvrResultNotify.mutable_oroomdata()->set_llminframes(m_llMinFrameMs);
	m_oSvrResultNotify.mutable_oroomdata()->set_iroomareanid(gBattleLogic->GetGroupId());

	for (size_t i = 0; i < m_vecResult.size(); ++i)
	{
		ProtoMsg::user_battle_result_t* pBattleResult = m_oSvrResultNotify.add_vecresult();
		if (pBattleResult != nullptr)
		{
			pBattleResult->CopyFrom(m_vecResult[i]);
		}
	}

	m_oCliResultNotify.mutable_vecdata()->CopyFrom(*(m_oSvrResultNotify.mutable_vecresult()));
	m_oCliResultNotify.set_ereusltcode(m_eResFail);
	
	//����
	SendAll(MsgModule_Battle::Msg_Battle_Result_Notify, ResultCode::Code_Common_Success, &m_oCliResultNotify);
}