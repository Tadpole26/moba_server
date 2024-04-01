#pragma once
#include "BaseInfoStruct.h"
namespace ProtoMsg
{
    class user_pb_data_t;
}

class CUser;
class CBaseInfoPackage
{
public:
    CBaseInfoPackage();
    ~CBaseInfoPackage();

public:
    void SetOwnerUser(CUser* pUserInfo) { m_pOwnerUser = pUserInfo; }
    void ParseBaseInfoFromDB(CUser* pUserInfo, const ProtoMsg::user_pb_data_t &oUserData);

    bool UpdateBaseInfoToDB();
    
    void SetUserId(int64_t llUserId) { m_stBaseInfo.m_llUserId = llUserId; }
    int64_t GetUserId() { return m_stBaseInfo.m_llUserId; }
    
    void SetAccount(const string& strAccount) { m_stBaseInfo.m_strAccount = strAccount; }
    string& GetAccount() { return m_stBaseInfo.m_strAccount; }
    
    void SetUserNick(const string& strUserNick) { m_stBaseInfo.m_strUserNick = strUserNick; }
    string& GetUserNick() { return m_stBaseInfo.m_strUserNick; }
    
    void SetGender(int iGender) { m_stBaseInfo.m_iGender = iGender; }
    int GetGender() { return m_stBaseInfo.m_iGender; }
    
    void SetUserLevel(int iUserLevel) { m_stBaseInfo.m_iUserLevel = iUserLevel; }
    int GetUserLevel() { return m_stBaseInfo.m_iUserLevel; }
    
    void SetUserExp(int iUserExp) { m_stBaseInfo.m_iUserExp = iUserExp; }
    int GetUserExp() { return m_stBaseInfo.m_iUserExp; }
    
    void SetCreateTime(time_t llCreateTime) { m_stBaseInfo.m_llCreateTime = llCreateTime; }
    time_t GetCreateTime() { return m_stBaseInfo.m_llCreateTime; }
    
    void SetLoginTime(time_t llLoginTime) { m_stBaseInfo.m_llLoginTime = llLoginTime; }
    time_t GetLoginTime() { return m_stBaseInfo.m_llLoginTime; }
    
    void SetLogoutTime(time_t llLogoutTime) { m_stBaseInfo.m_llLogoutTime = llLogoutTime; }
    time_t GetLogoutTime() { return m_stBaseInfo.m_llLogoutTime; }
    

private:
    CUser* m_pOwnerUser = nullptr;
    user_base_info_t m_stBaseInfo;
    
};