#include "CProcessorBase.h"
#include "log_mgr.h"
#include "msg_module_servercommon.pb.h"
#include "CGameLogic.h"
#include "CUser.h"

CProcessorBase::CProcessorBase(uint32_t uiModuleId)
	: m_uiModuleId(uiModuleId){ };

CProcessorBase::~CProcessorBase() { };

void CProcessorBase::SetUserBaseInfo(CUser* pUser, uint32_t uiCmd)
{
    m_pUser = pUser;
    m_uiCmd = uiCmd;
}

bool CProcessorBase::DoProcess(const std::string& strMsg)
{
    bool bRet = DoUserRun(strMsg);

    return bRet;
}

void CProcessorBase::SendToClient(uint32_t uiRspModule, uint32_t uiRspCmd, google::protobuf::Message* pRspMsg, ResultCode eCode, bool bNotify)
{
    Msg_ServerCommon_Transmit oTransmit;

    oTransmit.set_lluid(m_pUser->GetUserId());
    oTransmit.set_uimoduleid(uiRspModule);
    oTransmit.set_uicmd(uiRspCmd);
    oTransmit.set_uiretcode(eCode);
    oTransmit.set_uisequence(bNotify ? 0 : m_pUser->GetSequence());
    oTransmit.set_strcmdmsg(pRspMsg->SerializeAsString());
    if (gGameLogic->m_pGateSession)
        gGameLogic->m_pGateSession->Send_Msg(&oTransmit, MsgModule_ServerCommon::Msg_ServerCommon_Transmit);
} 
