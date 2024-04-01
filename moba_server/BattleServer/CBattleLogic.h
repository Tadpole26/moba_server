#pragma once
#include "singleton.h"
#include "logic_thread.h"
#include "CConstConfig.h"
#include "CCrossNetface.h"
#include "CLogNetface.h"
#include "server_logic_face.h"
#include "util_arg_opt.h"
#include "CConstConfig.h"
#include "func_proc.h"

#define TIME_OUT_SEC 10

class CBattleLogic;
class CWorldNetface;
class CLogNetFace;
class CBattleLogic
{
public:
	CBattleLogic();
	~CBattleLogic();

	int GetPlatId() { return m_oBattleConfig.m_iPlatId; }
	int GetGroupId() { return m_oBattleConfig.m_iGroupId; }
	int GetSvrId();
	std::string GetExIp() { return get_net_ip(); }

	void OnTimer(int iTime);

	int GetIndex() { return m_stArgOpt.GetIndex(); }

	bool Arg(int argc, char* argv[]);
	bool Launch();

protected:
	bool Init();
	bool Run();
	void Fini();
	void Stop();


public:
	CConstConfig	m_oBattleConfig;
	ArgOpt			m_stArgOpt;						//命令行参数信息

	CLogicThread* m_pLogic = nullptr;
	CCrossNetFace* m_pCrossNetFace = nullptr;
	CLogNetFace* m_pLogNetface = nullptr;
	CTimeEvent* m_pSvrEvent = nullptr;

	int32_t m_iMaxRoomNum = 50;
};

#define gBattleLogic Singleton<CBattleLogic>::getInstance()