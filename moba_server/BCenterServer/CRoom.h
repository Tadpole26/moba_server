#pragma once

#include <string>
#include "global_define.h"
#include "msg_module_serverinner.pb.h"
#include "CBattleCommon.h"
#include "result_code.pb.h"
class CBattleSession;
namespace ProtoMsg
{
	class Msg_ServerBattle_CB_CreateRoom_Req;
}
class CRoom
{
public:
	CRoom();
	virtual ~CRoom();

public:
	void Init();
	bool Init(ProtoMsg::Msg_ServerBattle_CB_CreateRoom_Req& oCreateReq);

	void Release();
	void ClearRoom();

	virtual void OnTimer(int iTime);

	void SetRoomState(ProtoMsg::ERoomStatus eState);

	void SetRoomId(int64_t llRoomId) { m_llRoomId = llRoomId; }
	int64_t GetRoomId() { return m_llRoomId; }

	ProtoMsg::ERoomType GetRoomType() const { return m_eRoomType; }
	void SetRoomType(ProtoMsg::ERoomType eType) { m_eRoomType = eType; }

	int GetMapId() const { return m_stMapInfo.m_iMapId; }
	const std::string& GetMapName() const { return m_stMapInfo.m_strMapName; }
	int GetMapSec() const { return m_stMapInfo.m_iSecond; }
	void SetMapInfo(battle_map_info_t stMapInfo) { m_stMapInfo = stMapInfo; }

	int GetInGameNum() const { return m_iInGameNum; }
	void SetInGameNum(int iVal) { m_iInGameNum = iVal; }

	int GetGroupId() const { return m_iGroupId; }
	void SetGroupId(int iVal) { m_iGroupId = iVal; }

	std::string GetIp() const { return m_strIp; }
	void SetIp(std::string strVal) { m_strIp = strVal; }

	std::string GetLinkIp() const { return m_strLinkIp; }
	void SetLinkIp(std::string strVal) { m_strLinkIp = strVal; }

	int GetPort() const { return m_iPort; }
	void SetPort(int iVal) { m_iPort = iVal; }

	bool IsOvered() { return m_bFinished; }
	bool IsTimeOut();
	int GetSubSec();				//返回剩余的秒数
	int GetSubWaitSec();			//返回等待创建的时间

	void SetBattleSvrId(int iVal) { m_iBattleSvrId = iVal; }
	int GetBattleSvrId() { return m_iBattleSvrId; }

	const std::string& GetRoomLogId() const { return m_strLogId; }
	void SetRoomLogId(std::string strLogId) { m_strLogId = strLogId; }

	int GetCrossSessionId() const { return m_iCrossSessionId; }
	void SetCrossSessionId(int iVal) { m_iCrossSessionId = iVal; }

	SERVER_KIND GetCrossType() const { return m_eCrossKind; }
	void SetCrossType(SERVER_KIND eVal) { m_eCrossKind = eVal; }

	int GetUserNum() { return m_iUserNum; }
	int GetRobotNum() { return m_iRobotNum; }

	void SetCreateProto(std::string strVal) { m_strCreateProto = strVal; }
	void SetCreateProto(const unsigned char* val, size_t uiSize)
	{
		m_strCreateProto.clear();
		m_strCreateProto.append((char*)val, uiSize);
	}
	void ClearCreateProto() { m_strCreateProto.clear(); }

	bool SendCreateRoom(CBattleSession* pBattle);
	void SendCreateRoomRes(ProtoMsg::ResultCode = ProtoMsg::Code_Match_FullRoom);

private:
	void DoOnverProcess();

protected:
	int64_t m_llRoomId = 0;
	ProtoMsg::ERoomType m_eRoomType = ProtoMsg::ERoomType_None;
	int m_iInGameNum = 0;
	int m_iUserNum = 0;
	int m_iRobotNum = 0;
	bool m_bFinished = false;
	int m_iCurTime = 0;												//累计时间

	int m_iRoomPeoNum = 0;											//房间人数
	battle_map_info_t m_stMapInfo;									//地图信息
	int m_iCrossSessionId = 0;
	SERVER_KIND m_eCrossKind = SERVER_KIND_NONE;

	int m_iBattleSvrId = 0;											//记录BattleSession的编号
	std::string m_strIp;
	std::string m_strLinkIp;
	int m_iPort = 0;
	int m_iGroupId = 0;

	std::string m_strLogId;											//当前战斗的唯一标识
	std::string m_strCreateProto;
	ProtoMsg::ERoomStatus m_eRoomState = ProtoMsg::ERoomStatus_None;
};