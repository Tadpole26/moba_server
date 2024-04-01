#pragma once

#include "singleton.h"
#include "logic_thread.h"
#include "zk_event.h"
#include "server_logic_face.h"
#include "CServerNetface.h"
#include "util_arg_opt.h"
#include "CConstConfig.h"


class CDBLogic
{
public:
	CDBLogic() { }
	~CDBLogic() { }
protected:
	bool Init();
	bool Run();
	void Fini();
	void Stop();
	void WritePidFile();
public:
	bool Arg(int argc, char* argv[]);
	bool Launch();

	void OnTimer(int iTime);
	uint32_t GetGroupId() { return m_oConstCfg.m_uiGroupId; }

public:
	CLogicThread* m_pLogic = nullptr;
	CTimeEvent* m_pDBPingEvent = nullptr;
	CClientLogic* m_pClientLIF = nullptr;

	CConstConfig  m_oConstCfg;
	ArgOpt		 m_stArgOpt;						//命令行参数信息


};

#define gDBLogic Singleton<CDBLogic>::getInstance()
