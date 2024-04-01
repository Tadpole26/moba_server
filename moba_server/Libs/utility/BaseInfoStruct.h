#pragma once
#include <string>
#include <iterator>
using namespace std;

/*
you need copy next code to msg_module_serverdb.proto file, and gen proto
Msg_ServerDB_GD_UpdateBaseInfo = x

message Msg_ServerDB_GD_UpdateBaseInfo
{
    int64 llUserId = 1;
    ServerDB_BaseInfo oInfo = 2;
}

message ServerDB_BaseInfo
{
    int64 llUserId = 1;                         //玩家唯一id
    string strAccount = 2;          //用户所属账号
    string strUserNick = 3;          //用户昵称
    int32 iGender = 4;          //用户性别
    int32 iUserLevel = 5;          //用户等级
    int32 iUserExp = 6;          //用户经验
    int64 llCreateTime = 7;                         //创角时间
    int64 llLoginTime = 8;                         //最近一次登录时间
    int64 llLogoutTime = 9;                         //最近一次登出时间
}
ServerDB_BaseInfo oBaseInfo = x
*/

struct user_base_info_t
{
    int64_t m_llUserId = 0;        //玩家唯一id
    string m_strAccount;
    string m_strUserNick;
    int m_iGender = 0;        //用户性别
    int m_iUserLevel = 0;        //用户等级
    int m_iUserExp = 0;        //用户经验
    time_t m_llCreateTime = 0;        //创角时间
    time_t m_llLoginTime = 0;        //最近一次登录时间
    time_t m_llLogoutTime = 0;        //最近一次登出时间
};