#include "CBaseInfoPackage.h"
#include "msg_module_serverdb.pb.h"
#include "CUser.h"
#include "CGameLogic.h"
using namespace ProtoMsg;

CBaseInfoPackage:: CBaseInfoPackage()
{
}

CBaseInfoPackage::~CBaseInfoPackage()
{
    m_pOwnerUser = nullptr;
}


void CBaseInfoPackage::ParseBaseInfoFromDB(CUser* pUserInfo, const ProtoMsg::user_pb_data_t &oUserData)
{
    m_pOwnerUser = pUserInfo;

    const ServerDB_BaseInfo& oData = oUserData.obaseinfo();
    SetUserId(oData.lluserid());
    SetAccount(oData.straccount());
    SetUserNick(oData.strusernick());
    SetGender(oData.igender());
    SetUserLevel(oData.iuserlevel());
    SetUserExp(oData.iuserexp());
    SetCreateTime(oData.llcreatetime());
    SetLoginTime(oData.lllogintime());
    SetLogoutTime(oData.lllogouttime());
    return;
}

bool CBaseInfoPackage::UpdateBaseInfoToDB()
{
    if (nullptr == m_pOwnerUser)
        return false;
    
    Msg_ServerDB_GD_UpdateBaseInfo oUpdate;
    oUpdate.set_lluserid(m_pOwnerUser->GetUserId());
    ServerDB_BaseInfo* pMsg = oUpdate.mutable_oinfo();
    if (pMsg == nullptr)
        return false;
    pMsg->set_lluserid(GetUserId());
    pMsg->set_straccount(GetAccount());
    pMsg->set_strusernick(GetUserNick());
    pMsg->set_igender(GetGender());
    pMsg->set_iuserlevel(GetUserLevel());
    pMsg->set_iuserexp(GetUserExp());
    pMsg->set_llcreatetime(GetCreateTime());
    pMsg->set_lllogintime(GetLoginTime());
    pMsg->set_lllogouttime(GetLogoutTime());
    if (gGameLogic->m_pDBNetFace)
        return gGameLogic->m_pDBNetFace->Send_Msg(&oUpdate, MsgModule_ServerDB::Msg_ServerDB_GD_UpdateBaseInfo, ServerDB);
    return false;
}
