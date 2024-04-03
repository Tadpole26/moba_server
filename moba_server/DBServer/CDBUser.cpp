#include "CDBUser.h"
#include "msg_module_serverdb.pb.h"
#include "DBTableNameStruct.h"
#include "CDBInstance.h"
using namespace ProtoMsg;

CDBUser::CDBUser()
{
}

CDBUser::~CDBUser()
{
}

void CDBUser::Init()
{
	m_llFix = GetCurrTime();
	m_bOnline = false;
}

void CDBUser::Release()
{

}

bool CDBUser::AddNewUser(const ProtoMsg::Msg_ServerDB_GD_CreateUser_Req& oCreateReq)
{
	const ServerDB_BaseInfo& oUserBase = oCreateReq.ouserinfo();

    m_oBaseInfoPkg.GetBaseInfo().m_llUserId = oUserBase.lluserid();
    m_oBaseInfoPkg.GetBaseInfo().m_strAccount = oUserBase.straccount();
    m_oBaseInfoPkg.GetBaseInfo().m_strUserNick = oUserBase.strusernick();
    m_oBaseInfoPkg.GetBaseInfo().m_iGender = oUserBase.igender();
    m_oBaseInfoPkg.GetBaseInfo().m_iUserLevel = oUserBase.iuserlevel();
    m_oBaseInfoPkg.GetBaseInfo().m_iUserExp = oUserBase.iuserexp();
    m_oBaseInfoPkg.GetBaseInfo().m_llCreateTime = oUserBase.llcreatetime();
    m_oBaseInfoPkg.GetBaseInfo().m_llLoginTime = oUserBase.lllogintime();
    m_oBaseInfoPkg.GetBaseInfo().m_llLogoutTime = oUserBase.lllogouttime();

    sDBRequest stRequest(eDB_InsertUpdate, table_name_t::user_table_name, o_db_baseinfo::tb_name, tagSecTblDesc::eST_Object);
    stRequest.AddCon(table_name_t::_id, oUserBase.lluserid());
    stRequest.AddReq(o_db_baseinfo::user_id, oUserBase.lluserid());
    stRequest.AddReqStr(o_db_baseinfo::account, oUserBase.straccount());
    stRequest.AddReqStr(o_db_baseinfo::user_nick, oUserBase.strusernick());
    stRequest.AddReq(o_db_baseinfo::gender, oUserBase.igender());
    stRequest.AddReq(o_db_baseinfo::user_level, oUserBase.iuserlevel());
    stRequest.AddReq(o_db_baseinfo::user_exp, oUserBase.iuserexp());
    stRequest.AddReqTime(o_db_baseinfo::create_time, oUserBase.llcreatetime());
    stRequest.AddReqTime(o_db_baseinfo::login_time, oUserBase.lllogintime());
    stRequest.AddReqTime(o_db_baseinfo::logout_time, oUserBase.lllogouttime());
    gDBInstance->PushGame(stRequest);
    return true;
}