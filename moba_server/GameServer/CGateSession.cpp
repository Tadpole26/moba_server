#include "CGateSession.h"
#include "log_mgr.h"
#include "parse_pb.h"
#include "CUser.h"
#include "CUserManager.h"
#include "msg_module_serverinner.pb.h"
#include "msg_module_servercommon.pb.h"
#include "CProcessorBase.h"
#include "CModuleProFactory.h"
#include "CGameLogic.h"
using namespace ProtoMsg;

CGateSession::CGateSession()
{
	SetServerKind(SERVER_KIND_NONE);
	SetServerId(0);
}

CGateSession::~CGateSession()
{
}

void CGateSession::on_disconnect()
{
	Log_Warning("GateSession %u disconnect!", m_uiServerId);
	if (m_eServerKind == SERVER_KIND_GATE)
		gUserManager->OnDisConnectGateServer();
	//g_SvrMgr.DelServer(m_eServerKind, m_uiServerId);

	//���ص�����Ϣ
	//if (GAME_LOGIC_INS->IsExit())
	//{
	//	CLoadData::GetInstance()->ServerExitRet(gGameLogic->GetAreanNo(), GetServerKind());
	//	gGameLogic->SetServerState(GetServerKind(), 2);
	//}
}

void CGateSession::handle_msg(const tagMsgHead* pNetMsg)
{
	if (!pNetMsg) return;
	uchar* pBuf = NET_DATA_BUF(pNetMsg);
	uint32 uiLen = NET_DATA_SIZE(pNetMsg);

	switch (pNetMsg->uiCmdId)
	{
	case MsgModule_ServerInner::Msg_ServerInner_GG_Login_Req:
		OnAccountEnter(pBuf, uiLen);
	break;
	case MsgModule_ServerInner::Msg_ServerInner_GG_Create_Req:
		OnCreatePlayer(pBuf, uiLen);
	break;
	case MsgModule_ServerInner::Msg_ServerInner_GG_Save_Ntf:
		OnSavePlayer(pBuf, uiLen);
	break;
	default:
		Log_Error("undefined module %d!", pNetMsg->usModuleId);
	break;
	}
}

void CGateSession::SendLoginErrorRsp(ProtoMsg::Msg_ServerInner_GG_Login_Req& oLoginReq, ProtoMsg::ResultCode eCode)
{
	Msg_ServerInner_GG_Login_Rsp oLoginRsp;
	oLoginRsp.set_lluserid(oLoginReq.lluserid());
	oLoginRsp.set_eflag(eCode);
	oLoginRsp.set_iseqid(oLoginReq.iseqid());
	oLoginRsp.set_brequest(false);
	oLoginRsp.set_estate(ProtoMsg::ePlayer_Offline);
	oLoginRsp.set_llopendate(gGameLogic->m_oConstCfg.m_llOpenTime);
	this->Send_Msg(&oLoginRsp, MsgModule_ServerInner::Msg_ServerInner_GG_Login_Rsp, MsgModule::ServerInner);
}

void CGateSession::OnAccountEnter(uchar* pMsg, uint32 uiLen)
{
	assert(pMsg);
	Msg_ServerInner_GG_Login_Req oLoginReq;
	PARSE_PTL(oLoginReq, pMsg, uiLen);
	Log_Custom("enter",  "account name=%s, account id=%lld", oLoginReq.straccname().c_str()
		, oLoginReq.lluserid());

	//�оɵ����ڼ�������,ɾ���ɵ�����
	CUser* pUser = gUserManager->GetCheckingUser(oLoginReq.lluserid());
	if (pUser != nullptr)
	{
		gUserManager->DelCheckingUser(oLoginReq.lluserid(), true);
		pUser = nullptr;
	}

	//�Ѿ���¼
	pUser = gUserManager->GetCheckedUser(oLoginReq.lluserid());
	if (pUser != nullptr)	
	{
		pUser->SetClientIp(oLoginReq.strclientip());
		pUser->SetSequence(oLoginReq.iseqid());
		if (oLoginReq.uirelogin())
		{
			if (pUser->GetDevId() != oLoginReq.strdeviceid())
				pUser->SendLoginRsp(Code_Login_DeviceNotSame);
			else
				pUser->SendLoginRsp(Code_Common_Success);
			return;
		}
	}

	//�Ƿ����ӳ�ɾ��map��
	if (pUser == nullptr)
		pUser = gUserManager->GetOutUser(oLoginReq.lluserid());

	if (pUser != nullptr)
	{
		pUser->SetSequence(oLoginReq.iseqid());
		pUser->SetClientIp(oLoginReq.strclientip());
		if (oLoginReq.uirelogin())
		{
			//if (!pUser->GetOnline() && (pUser->GetLeaveTimegGameLogic->m_oConstCfg.m_llOpenTime))
		}

		pUser->SetClientIp(oLoginReq.strclientip());
		pUser->SetDevId(oLoginReq.strdeviceid());
		pUser->EnterGame(oLoginReq.uirelogin() == 1);
		gUserManager->MoveCheckedUser(pUser);
		Log_Info("user account enter, user name=%s, user id=%lld", oLoginReq.straccname().c_str(), oLoginReq.lluserid());
		return;
	}

	//���������Ŷӷ�������,��׼����Ϸ
	if (oLoginReq.uirelogin())
	{
		SendLoginErrorRsp(oLoginReq, Code_Login_ReloginTimeout);
		Log_Custom("enter", "user account error relogin name:%s, userid:%lld", oLoginReq.straccname().c_str(), oLoginReq.lluserid());
		return;
	}

	//newһ���µ�user����
	pUser = gUserManager->CreateCheckingUser(oLoginReq.lluserid());
	if (pUser)
	{
		pUser->SetLeaveTime(GetCurrTime());
		pUser->SetClientIp(oLoginReq.strclientip());
		pUser->SetDevId(oLoginReq.strdeviceid());
	}
	else
	{
		SendLoginErrorRsp(oLoginReq, Code_Common_Failure);
	}
	return;
}

void CGateSession::OnCreatePlayer(uchar* pMsg, uint32 uiLen)
{
	assert(pMsg);
	Msg_ServerInner_GG_Create_Req oCreateReq;
	PARSE_PTL(oCreateReq, pMsg, uiLen);

	ResultCode eCode = ResultCode::Code_Common_Success;
	CUser* pUser = gUserManager->GetCheckingUser(oCreateReq.lluserid());
	if (pUser != nullptr)
	{
		Log_Error("can not find checking userid:%lld", oCreateReq.lluserid());
		return;
	}

	if (pUser->GetCreate())
	{
		Log_Error("have created userid:%lld", pUser->GetUserId());
		return;
	}

	Log_Custom("create", "create user, account:%s, userid:%lld", pUser->GetAccount().c_str(), oCreateReq.lluserid());

	if (pUser->GetUserId() != oCreateReq.lluserid())
	{
		Log_Error("can not checking userid:%lld", pUser->GetUserId());
		return;
	}
	
	pUser->SetSequence(oCreateReq.uiseqid());
	gGameLogic->m_pSysNetFace->ReqUniqueId(oCreateReq.lluserid(), oCreateReq.strusername(), gGameLogic->m_oConstCfg.m_uiGroupId);
}

void CGateSession::OnSavePlayer(uchar* pMsg, uint32 uiLen)
{
	assert(pMsg);
	Msg_ServerInner_GG_Save_Ntf oSaveReq;
	PARSE_PTL(oSaveReq, pMsg, uiLen);
}

void CGateSession::HandleTransmitData(const tagMsgHead* pNetMsg)
{
	Msg_ServerCommon_Transmit oTransmit;
	PARSE_PTL_HEAD(oTransmit, pNetMsg);

	CUser* pUser = gUserManager->GetCheckedUser(oTransmit.lluid());
	if (pUser != nullptr)
	{
		Log_Error("userid %lld not find! %u, cmd:%u", oTransmit.lluid(), oTransmit.uimoduleid(), oTransmit.uicmd());
		return;
	}
	pUser->SetSequence(oTransmit.uisequence());

	//������ͨ��Ϣ��
	processor_base_ptr_type pProcessor = PROCESSOR_FACTORY_INS.GetProcessor(oTransmit.uimoduleid());
	pProcessor->SetUserBaseInfo(pUser, oTransmit.uicmd());
	pProcessor->DoProcess(oTransmit.strcmdmsg());
	return;
}
