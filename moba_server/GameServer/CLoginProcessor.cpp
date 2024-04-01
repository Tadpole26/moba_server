#include "CLoginProcessor.h"
#include "CModuleProFactory.h"
#include "msg_module.pb.h"
#include "msg_module_login.pb.h"
#include "result_code.pb.h"
#include "CUser.h"
#include "CItemConfig.h"
using namespace ProtoMsg;

REGISTER_MODULE_FACTORY(MsgModule::Login, CLoginProcessor)

bool
CLoginProcessor::DoUserRun(const std::string& strMsg)
{
    if (m_uiCmd == MsgModule_Login::Msg_Login_GetItemList_Req)
    {
        Msg_Login_GetItemList_Rsp  oItemRsp;
        ResultCode eCode = (ResultCode)GetItemList(strMsg, oItemRsp);
        SendToClient(MsgModule::Login, MsgModule_Login::Msg_Login_GetItemList_Rsp, &oItemRsp, eCode);
    }
    else if (m_uiCmd == MsgModule_Login::Msg_Login_AwardItem_Req)
    {
        Msg_Login_AwardItem_Rsp oAwardRsp;
        ResultCode eCode = (ResultCode)AwardItem(strMsg, oAwardRsp);
        SendToClient(MsgModule::Login, MsgModule_Login::Msg_Login_AwardItem_Rsp, &oAwardRsp, eCode);
    }
    return (true);
}

int CLoginProcessor::GetItemList(const std::string& strMsg, Msg_Login_GetItemList_Rsp& oItemRsp)
{
    return Code_Common_Success;
}

int CLoginProcessor::AwardItem(const std::string& strMsg, Msg_Login_AwardItem_Rsp& oAwardRsp)
{
    Msg_Login_AwardItem_Req oAwardReq;
    PARSE_PTL_STR_RET_FAIL(oAwardReq, strMsg);

    return Code_Common_Success;
}





