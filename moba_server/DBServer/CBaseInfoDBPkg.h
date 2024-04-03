
#pragma once
#include "db_struct.h"
#include "BaseInfoStruct.h"
namespace ProtoMsg
{
    class Msg_ServerDB_GD_UpdateBaseInfo;
    class user_pb_data_t;
}
using namespace ProtoMsg;

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
};

class CDBUser;
class CBaseInfoDBPkg
{
public:
    static void OnUpdateBaseInfo(unsigned char* pMsg, size_t uiLen);

    static bool ReadDBBaseInfo(sDBSecRet& vecMap, CDBUser* pUser);

    void SetBaseInfo(user_pb_data_t& oData);
    user_base_info_t& GetBaseInfo() { return m_oBaseInfo; }
private:
    bool UpdateBaseInfo(Msg_ServerDB_GD_UpdateBaseInfo& oUpdate);
    bool AddOrUpdateDBBaseInfo(const Msg_ServerDB_GD_UpdateBaseInfo& oUpdate);

    user_base_info_t m_oBaseInfo;
};