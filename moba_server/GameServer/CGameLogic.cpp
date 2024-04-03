#include "CGameLogic.h"
#include "global_define.h"
#include "log_mgr.h"
#include "func_proc.h"
#include "global_define.h"
#include "coredump_x.h"
#include "util_file.h"
#include "mongo_base.h"
#include "CUserManager.h"
#include <fstream>

bool CGameLogic::Arg(int argc, char* argv[])
{
	return m_stArgOpt.Argv(argc, argv);
}

void CGameLogic::WritePidFile()
{
	std::string strFilePath;
#ifdef _WIN32
	strFilePath = get_exec_path() + "/tmp/";
#else 
	strFilePath = "/tmp/gundam/";
#endif
	create_dir(strFilePath.c_str());
	strFilePath += "_";
	strFilePath += std::to_string(m_oConstCfg.m_uiGroupId);
	strFilePath += ".pid";

	std::ofstream _ofs;
	_ofs.open(strFilePath.c_str(), std::ios_base::trunc | std::ios_base::out);
	_ofs << get_pid() << std::endl;
	_ofs.close();
}

bool CGameLogic::Init()
{
#ifdef _WIN32
	Log_Custom("start", "exe: %s ...", get_exec_name().c_str());
#else 
	Log_Custom("start", "exe: %s begin...", get_exec_name().c_str());
#endif
	//设置coredump文件和core回调
	InstallCoreDumper();
	SetConsoleInfo(std::bind(&CGameLogic::Stop, this));

	//读取本地配置
	if (!m_oConstCfg.Init())
	{
		Log_Error("load const config error!");
		return false;
	}

	//设置日志等级
#ifdef _WIN32
	GetLogMgrInstance()->SetTargetLevel(eTarConsole, eLevelDebug);
#else 
	GetLogMgrInstance()->SetTargetLevel(eTarConsole, eLevelWarning);
#endif
	GetLogMgrInstance()->SetTargetLevel(eTarFile, eLevelDebug);
	if (!GetLogMgrInstance()->Init(m_oConstCfg.m_uiGroupId, m_stArgOpt.GetIndex()))
	{
		Log_Error("log init error!!!");
		return false;
	}
	//初始化连接zk
	/*
	if (!m_stZkOpt.InitZookeeper(m_oConstCfg.m_strZkHost, m_oConstCfg.m_strNormalConfigPath))
	{
		Log_Error("init zookeeper error !!!");
		return false;
	}
	*/
	if (!gUserManager->Init())
	{
		Log_Error("init user manager error");
		return false;
	}

	if (is_listen_port(m_oConstCfg.m_uiGamePort))
	{
		Log_Error("listen port exsit %u!!!", m_oConstCfg.m_uiGamePort);
		return false;
	}

	net_setting stNetConfig;
	stNetConfig.m_nListenPort = m_oConstCfg.m_uiGamePort;
	//线程数量,每个线程可连接数量,接受,输入,输出包大小限制
	stNetConfig.m_ioThread.Init(1, 4096, ACCEPT_BUF_SIZE, SERVER_BUF_SIZE, SERVER_BUF_SIZE);
	stNetConfig.m_reThread.Init(get_cpu_num(), 20, ACCEPT_BUF_SIZE, SERVER_BUF_SIZE, SERVER_BUF_SIZE);

	m_pLogic = CLogicThread::newm(stNetConfig);
	if (m_pLogic == nullptr)
	{
		Log_Error("logic thread new failed!");
		return false;
	}

	m_pClientLIF = new CClientLogic();
	assert(m_pClientLIF);
	m_pLogic->reg_interface_listen(m_pClientLIF);

	m_pInterface = new CSvrLogicFace();
	assert(m_pInterface);
	m_pLogic->reg_interface_reconn(m_pInterface);

	m_pSysNetFace = new CSysNetface();
	m_pSysNetFace->SetServerID(m_oConstCfg.m_uiGroupId);
	if (m_pLogic->add_reconn(m_pSysNetFace, m_oConstCfg.m_strSysIp, m_oConstCfg.m_uiSysPort,
		SERVER_CON_SEC, MAX_SIZE_512M) < 0)
		return false;

	m_pDBNetFace = new CDBNetface();
	assert(m_pDBNetFace);
	if (m_pLogic->add_reconn(m_pDBNetFace, m_oConstCfg.m_strDBIp, m_oConstCfg.m_uiDBPort,
		SERVER_CON_SEC, MAX_SIZE_512M) < 0)
		return false;

	m_pCrossNetFace = new CCrossNetface();
	m_pCrossNetFace->SetServerID(m_oConstCfg.m_uiGroupId);
	if (m_pLogic->add_reconn(m_pCrossNetFace, m_oConstCfg.m_strCrossIp, m_oConstCfg.m_uiCrossPort,
		SERVER_CON_SEC, MAX_SIZE_512M) < 0)
		return false;

	regfn_io_recv_msg(my_io_recv_msg);
	regfn_io_send_msg(my_io_send_msg);
	/*
	m_pUpdateConfigEvent = new CZkEvent(m_pLogic->evthread()->Base(), true, m_stZkOpt.m_pZooHandle, 1000, nullptr);
	if (m_pUpdateConfigEvent == nullptr)
	{
		Log_Error("zookeeper event add failed!");
		return false;
	}

	if (!m_pUpdateConfigEvent->init())
	{
		Log_Error("zookeeper event init failed!");
		return false;
	}
	*/

	m_pTimerEvent = new CTimeEvent(m_pLogic->evthread()->Base(), true, 1000, std::bind(&CGameLogic::OnTimer, this, std::placeholders::_1));
	assert(m_pTimerEvent);
	if (!m_pTimerEvent->init())
		return false;
	Log_Info("success platid:%u, groupid:%u", m_oConstCfg.m_uiPlatId, m_oConstCfg.m_uiGroupId);
	return true;
}

bool CGameLogic::Run()
{
	WritePidFile();
	if (m_pLogic)
	{
		m_pLogic->run();
		return true;
	}
	return false;
}

void CGameLogic::Stop()
{
	if (m_pLogic != nullptr)
		m_pLogic->stop();
}

void CGameLogic::Fini()
{
	SAFE_DELETE(m_pSysNetFace);
	SAFE_DELETE(m_pCrossNetFace);
	SAFE_DELETE(m_pGateSession);
	SAFE_DELETE(m_pInterface);
	SAFE_DELETE(m_pGateSession);
	
	if (m_pLogic)
	{
		CLogicThread::del(m_pLogic);
		m_pLogic = nullptr;
	}
}

bool CGameLogic::Launch()
{
	if (!Init())
	{
		Stop();
		Fini();
		return false;
	}
	std::string strLocalIp = get_local_ip();
	Log_Custom("start", "local ip:%s", strLocalIp.c_str());
	bool bRet = Run();
	Fini();
	return bRet;
}

void CGameLogic::OnTimer(int iSec)
{
	SetCurrTime();
	gUserManager->OnTimer();
}