#pragma once
#include "log_mgr.h"
#include "lru_hashmap.h"
#include "result_code.pb.h"

#include "CBaseInfoPackage.h"
using namespace ProtoMsg;
namespace ProtoMsg
{
    class user_pb_data_t;
}


class CUser
{
public:
    CUser();
    ~CUser();

    void Init();
    void Release();
    void EnterGame(bool bRelogin = false);
    void LeaveGame(bool bSelfLeave = true);

public:
    int64_t GetUserId() { return 0; }

    std::string GetUserNick() { return m_strDevId; }
    std::string GetAccount() { return m_strDevId; }

    int GetUserLevel() { return m_iSequence; }
    int GetCreateTime() { return m_iSequence; }

    bool GetOnline() { return m_bOnline; }
    void SetOnline(bool bValue) { m_bOnline = bValue; }

    int GetSequence() { return m_iSequence; }
    void SetSequence(int iValue) { m_iSequence = iValue; }

    time_t GetLeaveTime() { return m_llLeaveTime; }
    void SetLeaveTime(time_t llValue) { m_llLeaveTime = llValue; }

    std::string GetClientIp() { return m_strClientIp; }
    void SetClientIp(const std::string& strValue) { m_strClientIp = strValue; }

    std::string GetDevId() { return m_strDevId; }
    void SetDevId(const std::string& strValue) { m_strDevId = strValue; }

    bool GetCreate() { return m_bCreate; }
    void SetCreate(bool bValue) { m_bCreate = bValue; }

    void SetUserData(const ProtoMsg::user_pb_data_t& oUserPbData);

    bool SendMsgClient(google::protobuf::Message* pMsg, uint8_t usModuleId, uint32 uiCmd,
            ProtoMsg::ResultCode eCode = ResultCode::Code_Common_Success, bool bNotify = false);
    bool SendMsgClient(const std::string& strMsg, uint8_t usModuleId, uint32 uiCmd,
        ProtoMsg::ResultCode eCode = ResultCode::Code_Common_Success, bool bNotify = false);

    void SendKickUserNotify(ProtoMsg::ResultCode eCode = Code_Login_RoleInGame);
    void SendLoginRsp(ProtoMsg::ResultCode eCode = Code_Common_Success);
    void SendCreateRsp(ProtoMsg::ResultCode eCode = Code_Common_Success);

    bool SendGetDBUserInfoReq();

    CBaseInfoPackage& GetBaseInfoPkg() { return m_oBaseInfoPkg; }

private:
    //内存数据  无需持久化
    int m_iSequence = 0;
    bool m_bOnline = false;
    time_t m_llLeaveTime = 0;
    std::string m_strClientIp = "";
    std::string m_strDevId = "";
    bool m_bCreate = false;

    //以下持久化数据库数据,老玩家直接在parsedb设值, 新玩家在InitNewUserData设置
    CBaseInfoPackage m_oBaseInfoPkg;
};

