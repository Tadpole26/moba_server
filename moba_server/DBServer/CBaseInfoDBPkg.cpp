#include "CBaseInfoDBPkg.h"
#include "CDBUserMgr.h"
#include "CDBUser.h"

const char* o_db_baseinfo::tb_name = "o_db_baseinfo";
const char* o_db_baseinfo::user_id = "user_id";
const char* o_db_baseinfo::account = "account";
const char* o_db_baseinfo::user_nick = "user_nick";
const char* o_db_baseinfo::gender = "gender";
const char* o_db_baseinfo::user_level = "user_level";
const char* o_db_baseinfo::user_exp = "user_exp";
const char* o_db_baseinfo::create_time = "create_time";
const char* o_db_baseinfo::login_time = "login_time";
const char* o_db_baseinfo::logout_time = "logout_time";


bool CBaseInfoDBPkg::ReadDBBaseInfo(sDBSecRet& vecMap, CDBUser* pUser)
{
    if (vecMap.empty()) return true;
    sDBMap& retMap = vecMap[0];

    stBaseInfo& stInfo = pUser->m_oBaseInfoOpt.GetBaseInfoInfo();
    stInfo.m_llUserId = retMap.(o_db_baseinfo::user_id);
    stInfo.m_strAccount = retMap.Str(o_db_baseinfo::account);
    stInfo.m_strUserNick = retMap.Str(o_db_baseinfo::user_nick);
    stInfo.m_iGender = retMap.NumInt(o_db_baseinfo::gender);
    stInfo.m_iUserLevel = retMap.NumInt(o_db_baseinfo::user_level);
    stInfo.m_iUserExp = retMap.NumInt(o_db_baseinfo::user_exp);
    stInfo.m_llCreateTime = retMap.Time(o_db_baseinfo::create_time);
    stInfo.m_llLoginTime = retMap.Time(o_db_baseinfo::login_time);
    stInfo.m_llLogoutTime = retMap.Time(o_db_baseinfo::logout_time);
    return true;
}

void CBaseInfoDBPkg::OnUpdateBaseInfo(unsigned char* pMsg, size uiLen)
{
    assert(pMsg);
    Msg_DBGS_UpdateBaseInfo oUpdate;
    PARSE_PTL(oUpdate, pMsg, uiLen);

    CDBUser* pUser = gDBUserManager->GetUserIn(oUpdate.lluserid());
    if (nullptr == pUser)
    {
        Log_Error("uer not exist!, uid:%lld", oUpdate.lluserid());
        return;
    }

    if (!pUser->m_oBaseInfoOpt.UpdateBaseInfo(oUpdate))
        Log_Error("uid:%lld update BaseInfo failed!", oUpdate.lluserid());
}
bool CBaseInfoDBPkg::UpdateBaseInfo(Msg_DBGS_UpdateBaseInfo& oUpdate)
{
    auto& oMsg = oUpdate.oinfo();
    m_oBaseInfo.m_llUserId = oMsg.lluserid();
    m_oBaseInfo.m_strAccount = oMsg.straccount();
    m_oBaseInfo.m_strUserNick = oMsg.strusernick();
    m_oBaseInfo.m_iGender = oMsg.igender();
    m_oBaseInfo.m_iUserLevel = oMsg.iuserlevel();
    m_oBaseInfo.m_iUserExp = oMsg.iuserexp();
    m_oBaseInfo.m_llCreateTime = oMsg.llcreatetime();
    m_oBaseInfo.m_llLoginTime = oMsg.lllogintime();
    m_oBaseInfo.m_llLogoutTime = oMsg.lllogouttime();

    AddOrUpdateDBBaseInfo(oUpdate);
    return true;
}
bool CBaseInfoDBPkg::AddOrUpdateDBBaseInfo(const Msg_DBGS_UpdateBaseInfo& oUpdate)
{
    auto& oInfo = oUpdate.oinfo();
    sDBRequest stRequest(eDB_InsertUpdate, db_game.u_player.tb_name, o_db_baseinfo::tb_name, tagSecTblDesc::eST_Object);

    stRequest.AddCon(db_game.u_player._id, oUpdate.lluserid());
    stRequest.AddReq(o_db_baseinfo::user_id, oInfo.lluserid());
    stRequest.AddReqStr(o_db_baseinfo::account, oInfo.straccount());
    stRequest.AddReqStr(o_db_baseinfo::user_nick, oInfo.strusernick());
    stRequest.AddReq(o_db_baseinfo::gender, oInfo.igender());
    stRequest.AddReq(o_db_baseinfo::user_level, oInfo.iuserlevel());
    stRequest.AddReq(o_db_baseinfo::user_exp, oInfo.iuserexp());
    stRequest.AddReqTime(o_db_baseinfo::create_time, oInfo.llcreatetime());
    stRequest.AddReqTime(o_db_baseinfo::login_time, oInfo.lllogintime());
    stRequest.AddReqTime(o_db_baseinfo::logout_time, oInfo.lllogouttime());
    gDBInstace->PushGame(stRequest);
    return true;
}
void CBaseInfoDBPkg::GetBaseInfo(DBGS_UserData& oData)
{
    ProtoMsg::DBGS_BaseInfo* pMsg = oData.mutable_obaseinfoinfo();
    if (pMsg == nullptr) return;
    pMsg->set_lluserid(m_oBaseInfo.m_llUserId);
    pMsg->set_straccount(m_oBaseInfo.m_strAccount);
    pMsg->set_strusernick(m_oBaseInfo.m_strUserNick);
    pMsg->set_igender(m_oBaseInfo.m_iGender);
    pMsg->set_iuserlevel(m_oBaseInfo.m_iUserLevel);
    pMsg->set_iuserexp(m_oBaseInfo.m_iUserExp);
    pMsg->set_llcreatetime(m_oBaseInfo.m_llCreateTime);
    pMsg->set_lllogintime(m_oBaseInfo.m_llLoginTime);
    pMsg->set_lllogouttime(m_oBaseInfo.m_llLogoutTime);
}