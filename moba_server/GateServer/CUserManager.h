#pragma once
#include "CUser.h"
#include <list>
#include "objects_pool.h"


using PLAYER_MAP = std::map<int64, CUser*>;

class CUserManager
{
public:
	CUserManager();
	~CUserManager();

	bool Init();
	bool GetLoadFinish() const { return m_bLoadFinish; }
	void SetLoadFinish(bool val) { m_bLoadFinish = val; }
public:
	//等待检测的玩家
	CUser* AddNewWaitCheckPlayer(int64 llUid);
	void AddWaitCheckPlayer(CUser* pPlayer);
	void DelWiatCheckPlayer(int64 llUid);
	CUser* GetWaitCheckPlayer(int64 llUid);
	size_t GetWaitCheckPlayerSize() { return m_lstWaitCheckPwd.size(); }

	//正在验证密码的操作的玩家
	CUser* GetCheckingPwdPlayer(int64 llUid);
	void DelCheckingPwdPlayer(int64 llUid);

	//加载游戏中的玩家
	CUser* GetCheckingPlayer(int64 llUid);
	void AddCheckingUser(int64_t llUid);

	//正在游戏中的玩家
	CUser* AddInGamePlayer(int64 llUid);
	void DelGamePlayer(int64 llUid);
	CUser* GetInGamePlayer(int64 llUid);
	size_t GetInGamePlayerSize() { return m_mapInGame.size(); }

	//定时任务
	void OnTimer(uint32 uiDwTm);
	void OnTimerCheckPwd();				//更新用户登录列表
	void OnTimerCheckingPwd();			//更新用户登录列表
	void OnTimerChecking();				//更新用户登录列表
	void OnTimerRandNotice();			//发送消息


	void PushThreadConn(CUser* pPlayer);


	CUser* GetPlayer(int64 llPlayerId);

	void SendToAllPlayer(const std::string& strMsg, msg_id_t usModule, msg_id_t usCmd, std::map<uint16, std::vector<uint16>>& stThreadToConnMap);
	void SendToAllPlayer(const tagMsgHead* pMyNetMsgHead);
	//间隔发送,只发最后一条新的
	void SendNoticeAllPlayer(const std::string& stMsg, msg_id_t usModule, msg_id_t usCmd, uint32 uiInterval);
	bool SendToPlayer(int64 llPlayerId, const tagMsgHead* pNetMsgHead);
	void ResetSecLogin();
	void AddSecLogin();
	void OnTimerLineUp();
	bool IsFull();
	bool ReLoginUser(CUser* pPlayer);

private:
	struct rand_notice_t
	{
		std::string m_strMsg;
		msg_id_t	m_usModule = 0;
		msg_id_t	m_usCmd = 0;
		uint32		m_uiInterval = 0;
		time_t		m_llLast = GetCurrTime();
	};

	std::map<int64, rand_notice_t> m_mapRandNotice;

	PLAYER_MAP m_mapInGame;											//正在游戏中的玩家
	PLAYER_MAP m_mapChecking;										//正在从游戏服务器数据的玩家
	PLAYER_MAP m_mapCheckingPwd;									//正在检验登录有效性
	std::list<CUser*> m_lstWaitCheckPwd;							//排队等待验证登录有效性
	std::map<uint16, std::vector<uint16>> m_mapThreadToConn;		//线程id对应的玩家连接id集合

	ObjectsPool<CUser>* m_pUserPool = nullptr;						//玩家内存池

	NumTimer<60000> m_stOnLineUp;
	NumTimer<10> m_stLineUp;
	NumTimer<1000> m_stRandNotice;

	uint32 m_uiSecLogin = 0;										//当前秒已经登录的数量
	uint32 m_uiPreSecLogin = 0;										//上一秒已经登录的数量
	time_t m_llPreLogin = GetCurrTime();	

	bool m_bLoadFinish = false;

};