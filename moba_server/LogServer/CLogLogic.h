#pragma once
#include "singleton.h"
#include "logic_thread.h"
#include "util_arg_opt.h"
#include "server_logic_face.h"
#include "CServerNetface.h"
#include "CConstConfig.h"

class CLogLogic
{
public:
	CLogLogic();
	~CLogLogic();

public:
	ArgOpt					m_stArgOpt;						//命令行参数信息
	CConstConfig			m_oConstCfg;					//本地配置
	std::string				m_strExIp;

	CLogicThread* m_pLogic = nullptr;
	CTimeEvent* m_pDBPingEvent = nullptr;
	CClientLogic* m_pClientLIF = nullptr;

	bool Arg(int argc, char* argv[]);
	bool Launch();

protected:
	bool Init();
	bool Run();
	void Fini();
	void Stop();

	void OnTimer(uint32 dwTm);

	void WritePidFile();
};

#define LOG_LOGIC_INS Singleton<CLogLogic>::getInstance()
