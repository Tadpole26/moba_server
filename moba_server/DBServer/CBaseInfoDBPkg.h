
#pragma once
#include "dtype.h"
#include "BaseInfoStruct.h"

struct o_db_baseinfo
{
    static const char* tb_name;
    static const char* user_id;
    static const char* account;
    static const char* user_nick;
    static const char* gender;
    static const char* user_level;
    static const char* user_exp;
    static const char* create_time;
    static const char* login_time;
    static const char* logout_time;
}

class CDBUser;
class CBaseInfoDBPkg
{
public:
    static void OnUpdateBaseInfo(unsigned char* pMsg, size uiLen);

    static bool ReadDBBaseInfo(sDBSecRet& vecMap, CDBUser* pUser);

    void GetBaseInfo(DBGS_UserData& oData);
    stBaseInfo& GetBaseInfoInfo() { return m_oBaseInfo; }
private:
    bool UpdateBaseInfo(Msg_DBGS_UpdateBaseInfo& oUpdate);
    bool AddOrUpdateDBBaseInfo(const Msg_DBGS_UpdateBaseInfo& oUpdate);
    stBaseInfo m_oBaseInfo;
};