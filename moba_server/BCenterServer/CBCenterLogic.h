#pragma once
#include "singleton.h"
#include "logic_thread.h"
#include "server_logic_face.h"
#include "CServerNetface.h"
#include "CCrossSession.h"
#include "util_arg_opt.h"
#include "CConstConfig.h"

class CCrossSession;
class CBCenterLogic
{
public:
	CBCenterLogic() { }
	~CBCenterLogic() { }

	int GetIndex() { return m_stArgOpt.GetIndex(); }
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
	CCrossSession* m_pCrossSession = nullptr;			//����ֻ��Ҫһ��cross server����bcenter server

	CConstConfig  m_oConstCfg;
	ArgOpt		 m_stArgOpt;						//�����в�����Ϣ

	CSvrLogicFace* m_pServerLIF = nullptr;

};

#define gBCenterLogic Singleton<CBCenterLogic>::getInstance()