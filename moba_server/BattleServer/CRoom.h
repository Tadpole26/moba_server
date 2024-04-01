#pragma once
#include <cstdint>
#include <string>
#include "msg_common.pb.h"
#include "msg_module_battle.pb.h"
#include "msg_module_serverinner.pb.h"
#include "CBattleCommon.h"
#include "msg_make.h"

namespace ProtoMsg
{
	class user_battle_info_t;
	class user_battle_result_t;
	class battle_order_t;
	class Msg_ServerInner_CB_CreateRoom_req;
}

class CBattleUser;
class CRoomThread;
class CRoom : public CMsgMake
{
public:
	CRoom();
	~CRoom();

	void Init();
	void InitRoomData(ProtoMsg::Msg_ServerInner_CB_CreateRoom_req& oCreateReq);
	void Release();

	int GetIndex() const { return m_iIndex; }
	void SetIndex(int iIndex) { m_iIndex = iIndex; }

	int64_t GetRoomId() const { return m_llRoomId; }
	void SetRoomId(int64_t llRoomId) { m_llRoomId = llRoomId; }

	const std::string& GetLogId() const { return m_strLogId; }
	void SetLogId(const std::string& strLogId) { m_strLogId = strLogId; }

	bool IsUdp() const { return m_bUdp; }
	void SetUdp(bool val) { m_bUdp = val; }

	int GetHostPort() const { return m_iPort; }
	void SetHostPort(int val) { m_iPort = val; }

	int GetUserNum() const { return m_iUserNum; }
	int GetInUserNum() const { return (int)m_setUserIn.size(); }
	int GetUserDataNum() const { return (int)m_vecUsersData.size(); }
	int GetObUserDataNum() const { return (int)m_vecObUsersData.size(); }

	ProtoMsg::ERoomStatus GetRoomStatus() const { return m_eRoomStatus; }

	bool IsRun() { return m_eRoomStatus == ProtoMsg::ERoomStatus_Run; }
	bool IsEnd() { return  m_eRoomStatus == ProtoMsg::ERoomStatus_End; }
	int GetAllFrame() { return m_iFrameCount; }

	CRoomThread* GetRoomNet() const { return m_pRoomNet; }
	void SetRoomNet(CRoomThread* val) { m_pRoomNet = val; }

	std::map<int64_t, int>& GetConvList() { return m_mapUserConvId; }

	ProtoMsg::user_battle_info_t* GetUserData(std::string strSessionId, bool bAll = false);
	ProtoMsg::user_battle_info_t* GetUserDataById(int64_t llUid, bool bAll = false);

	//战斗开始
	void OnBattleStart(int64_t llTick, bool bFirst = true);
	//玩家进入房间
	void Enter(CBattleUser* pBtlUser);
	//玩家离开房间
	void Leave(CBattleUser* pBtlUser);
	void KillUser(CBattleUser* pBtlUser, int64_t llTick);
	void DeleteUser(CBattleUser* pBtlUser);
	void KickAllUser(int64_t llTick);
	//该玩家是否战斗结束并上传战报
	bool IsOver(size_t uiPos) { return !m_vecUserResult[uiPos].empty(); }
	//增加玩家操作指令(离开房间等等)
	void AddOrder(const ProtoMsg::battle_order_t& stData, CBattleUser* pBtlUser = nullptr);
	//统计房间已经准备玩家
	int ReadySize();
	//统计房间已经准备好的观战玩家
	int ReadyObSize();
	//所有玩家是否发送指令结束且战斗结束(战斗结束中上传战报)
	bool IsAllOver();
	//房间玩家id拼接成唯一字符串
	const std::string& MakeIdList();
	//客户端请求房间数据回复
	void SendEnterRoom(CBattleUser* pBtlUser);
	//
	void SendLoadProgress(CBattleUser* pBtlUser);
	//广播给房间所有在线玩家
	void SendAll(int iMsgId, ProtoMsg::ResultCode eCode, google::protobuf::Message* pMsg, bool bAll = true);
	//是否超时
	bool IsTimeOut(int64_t llTick);

	//帧同步数据给客户端
	void OnDoHeart(int64_t llTick);
	void OnHeartbeat(int64_t llTick);
	int OnTimer(int64_t llTick);

	std::vector<ProtoMsg::user_battle_result_t>& GetBattleResult() { return m_vecResult; }
	ProtoMsg::user_battle_result_t* GetResult(int64_t llUid);
	void AddResult(int iPos, std::vector<ProtoMsg::user_battle_result_t>& vecRes, std::string& strResult, int64_t llUid);
	void AddKeyType(size_t uiPos, int ikeyLst);
	size_t GetResultPos(std::string strResult);
	void CheckResult();
	//上传战报是否全部平局
	bool CheckResultPvg();		
	//上传战报是否一方胜利
	bool CheckResultWin();
	//上传战报是否一致
	bool CheckResultPerson();
	void MakeEndResult(int64_t llTick);

	void DoEnd();
	void PushEnd(int64_t llTick);

private:
	CRoomThread* m_pRoomNet = nullptr;

	bool m_bUdp = false;
	int m_iIndex = -1;
	int m_iPort = 0;
	int m_iRandSeed = 0;										//房间随机数
	ProtoMsg::ERoomType m_eRoomType = ProtoMsg::ERoomType_None;				//房间类型
	int64_t m_llRoomId = 0;											//房间id
	ProtoMsg::ERoomStatus m_eRoomStatus = ProtoMsg::ERoomStatus_None;		//房间状态
	battle_map_info_t m_stMapInfo;									//地图信息
	std::string m_strLogId;
	int m_iUserNum = 0;													//房间的玩家数
	ProtoMsg::ELeisureType m_eLeisureType = ProtoMsg::ELeisureType_None;			//休闲模式类型

	int64_t m_llCreateTime = 0;							//创建房间时间
	int64_t m_llBeginTime = 0;							//战斗开始时间
	int64_t m_llEndTime = 0;							//战斗结束时间
	int64_t m_llLastLeaveTime = 0;						//最近一个人离开的时间点

	std::vector<ProtoMsg::user_battle_info_t> m_vecUsersData;		//在房间玩家数据(创建房间就有,一直存在)
	std::vector<ProtoMsg::user_battle_info_t> m_vecObUsersData;		//在房间观战玩家数据(创建房间就有,一直存在)
	std::string m_strUserList;							//在房间玩家userid拼接的字符串
	std::set<CBattleUser*> m_setUserIn;					//在房间战斗玩家(socket连接的玩家,掉线什么不在里面)
	std::set<CBattleUser*> m_setObUserIn;				//在房间观战玩家(socket连接的玩家,掉线什么不在里面)
	std::map<int64_t, time_t> m_mapUserOff;				//保存掉线玩家
	std::map<int64_t, time_t> m_mapHasEnter;			//保存进入过游戏玩家
	std::set<int64_t> m_setEscape;						//逃跑玩家
	std::vector<int64_t> m_vecRobotsIn;					//机器人
	std::map<int64_t, int>	m_mapUserConvId;			//玩家udp中convid集合

	/////////////////////////////////////////////////////////////////////////////////////
	int64_t m_llPreFrameTime = 0;													//上一帧广播的时间
	int m_iEmptyFrame = 0;															//空帧
	int64_t m_llMaxFrameMs = 0;
	int64_t m_llMinFrameMs = 0;
	int		m_iFrameCount = 0;														//总共经历的帧数
	int		m_iNetFrame = 0;														//下一次帧执行到的准确时间点
	::google::protobuf::RepeatedPtrField<ProtoMsg::battle_order_t> m_vecCurOrders;	//当前帧操作集合	
	std::vector<ProtoMsg::battle_order_t>	m_vecRecOrders;							//房间所有帧操作集合
	std::vector<int> m_vecRecpos;													//每帧动作位置
	////////////////////////////////////////////////////////////////////////////////////


	///////////////////////////////////////////////////////////////////////////////////
	ProtoMsg::EBRCode m_eResFail = ProtoMsg::EBRCode_Success;						//战斗战报返回
	//正确结果
	std::vector<ProtoMsg::user_battle_result_t> m_vecResult;	//存储玩家上报的战报
	std::string m_strResult;
	std::vector<int> m_vecKeyType;

	//玩家上报的结果
	std::vector<std::string> m_vecStrResult;
	std::map<std::string, int> m_mapResultTimes;	//玩家上传的所有玩家战报序列化成string对应上传次数
	std::vector<std::vector<ProtoMsg::user_battle_result_t>> m_vecUserResult;	//位置对应上传的多份战报(每个人都会上传所有玩家的战报)
	std::set<int64_t> m_setUserOver;
	int m_iMustResNum = 0;
	int m_iHaveOverNum = 0;											//上报的玩家数
	time_t m_llOverBegin = 0;										//是否已经有玩家上报结果

	ProtoMsg::Msg_RoomHeartbeat_Notify m_oCliBeatNorify;
	ProtoMsg::Msg_Battle_Result_Notify m_oCliResultNotify;
	ProtoMsg::Msg_ServerInner_BC_Result_Notify m_oSvrResultNotify;
	////////////////////////////////////////////////////////////////////////////////////
};