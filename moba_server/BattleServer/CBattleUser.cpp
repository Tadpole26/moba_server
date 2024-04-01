#include "CBattleUser.h"
#include "log_mgr.h"
#include "CBattleLogic.h"
#include "CRoom.h"
#include "msg_module_battle.pb.h"
#include "result_code.pb.h"
#include "parse_pb.h"

CBattleUser::CBattleUser()
{
}

CBattleUser::~CBattleUser()
{
}

void CBattleUser::Init()
{
}

void CBattleUser::Release()
{
	m_bTcp = false;
	m_pRoom = nullptr;
	m_iLoadProgress = 0;
	m_llRoomId = 0;

	m_vecResult.clear();
	m_strUserName.clear();
	m_strSessionid.clear();

	m_oOverMsg.Clear();
	m_strResult.clear();
	m_strIp.clear();
}

void CBattleUser::Kill(int64_t llTick)
{
	m_bInit = false;
	m_bBeginKill = true;

	if (m_pRoom != nullptr)
		m_pRoom->KillUser(this, llTick);
}

void CBattleUser::handle_msg(const tagMsgHead* pHead)
{
	m_iSequenceId = pHead->uiSeqid;

	switch (pHead->uiCmdId)
	{
	case MsgModule_Battle::Msg_Battle_EnterRoom_Req:
		OnMsgEnterRoomReq(pHead);
		break;
	default:
		Log_Error("unknwn register userid:%lld, cmd:%u, ip:%s", m_llUid, pHead->uiCmdId, m_strIp.c_str());
		if (m_bBeginKill) return;
		Kill();
		break;
	}
}

bool CBattleUser::IsTimeOut()
{
	if (m_pTcpUdpSess)
		return m_pTcpUdpSess->IsOut();
	return false;
}

void CBattleUser::on_disconnect()
{
	CRoom* pRoom = m_pRoom;
	if (pRoom != nullptr)
	{
		Log_Custom("enter", "roomid:%lld, logid:%s, name:%s, uid:%lld, timeout:%d, little:%u, big:%u",
			pRoom->GetRoomId(), pRoom->GetLogId().c_str(), m_strUserName.c_str(), m_llUid, IsTimeOut(),
			m_uiLittleOrderMs, m_uiBigOrderMs);
		m_pRoom = nullptr;
		m_bInit = false;
		m_bBeginKill = true;

		m_pRoom->Leave(this);
	}
	else
	{
		Log_Custom("enter", "name:%s, uid:%lld, timeout:%d, little:%u, big:%u",
			m_strUserName.c_str(), m_llUid, IsTimeOut(), m_uiLittleOrderMs, m_uiBigOrderMs);
	}
}

CTcpSess* CBattleUser::InitTcp()
{
	m_bInit = false;
	m_bBeginKill = false;
	m_bTcp = true;
	m_pTcpUdpSess = &m_oTcpSess;

	int64_t llTick = GetMillSec();
	m_pTcpUdpSess->SetLastTime(GetCurrTime());
	m_pTcpUdpSess->SetConvId(GetConvId());
	m_pTcpUdpSess->InitTcpUdp(llTick);
	m_pTcpUdpSess->SetBtlUser(this);
	m_pTcpUdpSess->SetUserId(GetUserId());
	m_pTcpUdpSess->Tick(llTick);
	return dynamic_cast<CTcpSess*>(m_pTcpUdpSess);
}

CUdpSess* CBattleUser::InitUdp()
{
	m_bInit = false;
	m_bBeginKill = false;
	m_bTcp = true;
	m_pTcpUdpSess = &m_oUdpSess;

	int64_t llTick = GetTickCount();
	m_pTcpUdpSess->SetLastTime(GetCurrTime());
	m_pTcpUdpSess->SetConvId(GetConvId());
	m_pTcpUdpSess->InitTcpUdp(llTick);

	m_pTcpUdpSess->SetBtlUser(this);
	m_pTcpUdpSess->SetUserId(GetUserId());
	m_pTcpUdpSess->Tick(llTick);

	return dynamic_cast<CUdpSess*>(m_pTcpUdpSess);
}

void CBattleUser::SetRoom(CRoom* pRoom)
{
	m_pRoom = pRoom;
	if (pRoom != nullptr)
		m_llRoomId = m_pRoom->GetRoomId();
}

void CBattleUser::SetUserBaseData(ProtoMsg::user_battle_info_t& oUserData)
{
	m_llUid = oUserData.ouserinfo().lluid();
	m_strUserName = oUserData.ouserinfo().strname();
	m_bOb = oUserData.bob();
	m_strSessionid = oUserData.strusersessionid();
}

void CBattleUser::InitIp()
{
	if (m_bTcp)
		m_strIp = m_oTcpSess.get_ip_str();
	else
		m_strIp = m_oUdpSess.GetIpStr();
}

std::string CBattleUser::GetIp()
{
	if (!m_strIp.empty()) return m_strIp;

	if (m_bTcp)
		m_strIp = m_oTcpSess.get_ip_str();
	else
		m_strIp = m_oUdpSess.GetIpStr();

	return m_strIp;
}

void CBattleUser::SendProtol(google::protobuf::Message* pMsg, msg_id_t usCmd, int iCode, int iSequence)
{
	if (m_pTcpUdpSess != nullptr)
		m_pTcpUdpSess->Send_Msg(pMsg, usCmd, MsgModule::Battle, (ResultCode)iCode, iSequence);
}

void CBattleUser::OnMsgEnterRoomReq(const tagMsgHead* pHead)
{
	if (m_pTcpUdpSess->IsTcp()) return;

	Msg_Battle_EnterRoom_Req oEnterReq;
	PARSE_PTL_HEAD(oEnterReq, pHead);

	if (m_pRoom == nullptr || m_pRoom->IsEnd() || m_pRoom->IsAllOver()
		|| m_pRoom->GetLogId() != oEnterReq.strroomsessionid() || m_strSessionid != oEnterReq.strusersessionid())
	{
		Msg_Battle_EnterRoom_Rsp oEnterRsp;
		oEnterRsp.set_strmapname("");
		oEnterRsp.set_etype(ERoomType::ERoomType_None);
		oEnterRsp.set_irandseed(0);
		SendProtol(&oEnterRsp, MsgModule_Battle::Msg_Battle_EnterRoom_Rsp, ResultCode::Code_Match_RoomRelease, m_iSequenceId);
		Kill(GetTickCount());
		return;
	}

}

void CBattleUser::OnEnterRoom(Msg_Battle_EnterRoom_Req& oEnterReq, int iSequence)
{
	m_iSequenceId = iSequence;
	m_bInit = true;
	m_bBeginKill = false;

	if (m_pRoom->IsRun() && m_pRoom->GetAllFrame() > 0)
		m_bLoadEnd = false;

	if (!IsOb()) m_bOver = m_pRoom->IsOver(GetPosition());

	m_pRoom->SendEnterRoom(this);
	m_pRoom->SendLoadProgress(this);

	Log_Custom("enter", "enter roomid:%lld, logid:%s, user name:%s, userid:%lld, conv:%d, ip:%s",
		m_pRoom->GetRoomId(), m_pRoom->GetLogId().c_str(), GetUserName().c_str(), GetUserId(), m_strIp.c_str());
}