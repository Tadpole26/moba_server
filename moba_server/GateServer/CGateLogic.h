#pragma once
#include "singleton.h"
#include "logic_thread.h"
#include "CUserManager.h"
#include "CConstConfig.h"
#include "util_arg_opt.h"
#include "server_logic_face.h"


class CGameNetface;
class CSysNetface;
class CClientLogic;
class CGateLogic
{
public:
	CGateLogic() {}
	~CGateLogic() {}

protected:
	bool Init();
	bool Run();
	void Fini();
	void Stop();

	void OnTimer(uint32 dwTm);
	void WritePidFile();

public:
	bool Arg(int argc, char* argv[]);
	bool Launch();

	uint32 GetMaxSend() { return m_uiMaxSend; }
	bool SetMaxSend(uint32 uiSize);

	int GetIndex() { return m_stArgOpt.GetIndex(); }

	std::string& GetResVersion() { return m_strResVersion; }
	void SetResVersion(std::string strVersion) { m_strResVersion = strVersion; }

	void SendWChatMsg(const std::string& strClient
		, const std::string& strServer, const std::string& strMsg);

public:
	ArgOpt					m_stArgOpt;						//�����в�����Ϣ
	CConstConfig			m_oConstConfig;					//��������

	CLogicThread*			m_pLogic = nullptr;
	CUserManager			m_oUserMgr;
	CClientLogic*			m_pClientLIF = nullptr;			//�����û����Ӽ���(bufferevent)
	CSvrLogicFace*			m_pServersLIF = nullptr;		//�������ӷ���������

	CSysNetface*			m_pSysNetface = nullptr;
	CGameNetface*			m_pGameNetface = nullptr;
private:
	CTimeEvent*				m_pUserUpEvent = nullptr;
	CTimeEvent*				m_pEvent = nullptr;
	uint32					m_uiMaxSend = 0;
	std::string				m_strResVersion;
};

#define gGateLogic Singleton<CGateLogic>::getInstance()