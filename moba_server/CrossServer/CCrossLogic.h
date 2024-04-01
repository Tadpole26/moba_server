#pragma once
#include "singleton.h"
#include "logic_thread.h"
#include "zk_event.h"
#include "CConstConfig.h"
#include "server_logic_face.h"
#include "util_arg_opt.h"
#include "CServerNetface.h"

class CCrossLogic
{
public:
	CCrossLogic() { }
	~CCrossLogic() { }
protected:
	bool Init();
	bool Run();
	void Fini();
	void Stop();
	void OnTimer(uint32 uiSec);
	void WritePidFile();
public:
	bool Arg(int argc, char* argv[]);
	bool Launch();

public:
	CLogicThread* m_pLogic = nullptr;
	CClientLogic* m_pClientLIF = nullptr;

	CSvrLogicFace* m_pReconInterface = nullptr;

	CConstConfig  m_oConstCfg;
	ArgOpt		 m_stArgOpt;						//命令行参数信息

};

#define CROSS_LOGIC_INS Singleton<CCrossLogic>::getInstance()