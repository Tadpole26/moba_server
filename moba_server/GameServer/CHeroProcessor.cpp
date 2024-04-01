#include "CHeroProcessor.h"
#include "CModuleProFactory.h"
#include "log_mgr.h"
#include "msg_module.pb.h"

using namespace ProtoMsg;

REGISTER_MODULE_FACTORY(MsgModule::Hero, CHeroProcessor)

bool
CHeroProcessor::DoUserRun(const std::string& strMsg)
{
    if (m_uiCmd == MsgModule_Hero::Msg_Hero_UpHeroLevel_Req)
    {
        Msg_Hero_UpHeroLevel_Rsp  oLevelRsp;
        ResultCode eCode = (ResultCode)UpHeroLevel(strMsg, oLevelRsp);
        SendToClient(MsgModule::Hero, MsgModule_Hero::Msg_Hero_UpHeroLevel_Rsp, &oLevelRsp, eCode);
    }
    else if (m_uiCmd == MsgModule_Hero::Msg_Hero_UpHeroStar_Req)
    {
    }
    else
    {
        Log_Error("module:hero cmd:%u is not exist", m_uiCmd);
    }
    return (true);
}

int CHeroProcessor::UpHeroLevel(const std::string& strMsg, Msg_Hero_UpHeroLevel_Rsp& oLevelRsp)
{
    return Code_Common_Success;
}

int CHeroProcessor::UpHeroStar(const std::string& strMsg, Msg_Hero_UpHeroStar_Rsp& oStarRsp)
{
    return Code_Common_Success;
}







