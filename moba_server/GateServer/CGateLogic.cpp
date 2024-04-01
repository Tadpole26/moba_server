#include <thread>
#include <fstream>

#include "CGateLogic.h"
#include "global_define.h"
#include "CGameNetface.h"
#include "CSysNetface.h"
#include "CServerNetface.h"
#include "log_mgr.h"
#include "util_file.h"
#include "func_proc.h"
#include "coredump_x.h"

bool CGateLogic::Arg(int argc, char* argv[])
{
	return m_stArgOpt.Argv(argc, argv);
}

void CGateLogic::WritePidFile()
{
	std::string strFilePath;
#ifdef _WIN32
	strFilePath = get_exec_path() + "/tmp/";
#else 
	strFilePath = "/tmp/gundam/";
#endif
	create_dir(strFilePath.c_str());
	strFilePath += "_";
	strFilePath += std::to_string(m_oConstConfig.m_uiGroupId);
	strFilePath += ".pid";

	std::ofstream _ofs;
	_ofs.open(strFilePath.c_str(), std::ios_base::trunc | std::ios_base::out);
	_ofs << get_pid() << std::endl;
	_ofs.close();
}

bool CGateLogic::Launch()
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

bool CGateLogic::Init()
{
#ifdef _WIN32
	Log_Custom("start", "exe: %s ...", get_exec_name().c_str());
#else 
	Log_Custom("start", "exe: %s, index:%d, begin...", get_exec_name().c_str(), GetIndex());
#endif
//设置coredump文件和core回调
	InstallCoreDumper();
	SetConsoleInfo(std::bind(&CGateLogic::Stop, this));

//读取本地配置
	if (!m_oConstConfig.Init())
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
	if (!GetLogMgrInstance()->Init(m_oConstConfig.m_uiGroupId, m_stArgOpt.GetIndex()))
	{
		Log_Error("log init error!!!");
		return false;
	}

//启动网络监听和事件循环
	net_setting stNetConfig;
	stNetConfig.m_nListenPort = m_oConstConfig.m_uiGatePort;
	if (is_listen_port(stNetConfig.m_nListenPort))
	{
		Log_Error("listen port exsit %u!!!", stNetConfig.m_nListenPort);
		return false;
	}
	//连接网关人数限制
	int iClientLimit = m_oConstConfig.m_uiLimit;
	if (iClientLimit < 1024)
		iClientLimit = 1024;

	if (!m_oUserMgr.Init())
	{
		Log_Error("playermgr init error size=%d!", iClientLimit);
		return false;
	}

	stNetConfig.m_ioThread.Init(get_cpu_num(), iClientLimit
			, ACCEPT_BUF_SIZE, SERVER_BUF_SIZE, SERVER_BUF_SIZE
			, 512, 120);
	Log_Custom("start", "io thread num:%u, client limit num:%u", get_cpu_num(), iClientLimit);
	stNetConfig.m_reThread.Init(4, 10, ACCEPT_BUF_SIZE
			, SERVER_BUF_SIZE, SERVER_BUF_SIZE);
	Log_Custom("start", "re thread num:%u, client limit num:%u", 4, 10);
	//开启监听、io、逻辑线程
	m_pLogic = CLogicThread::newm(stNetConfig);
	if (m_pLogic == NULL)
	{
		Log_Error("logic thread new failed !!!");
		return false;
	}
	//管理所有用户结构(bufferevent)
	m_pClientLIF = new CClientLogic();
	assert(m_pClientLIF);
	m_pLogic->reg_interface_listen(m_pClientLIF);

	m_pServersLIF = new CSvrLogicFace();
	m_pLogic->reg_interface_reconn(m_pServersLIF);

	m_pSysNetface = new CSysNetface();
	m_pSysNetface->SetServerID(m_oConstConfig.m_uiGroupId);
	ERROR_EXIT(m_pLogic->add_reconn(m_pSysNetface, m_oConstConfig.m_strSysIp, m_oConstConfig.m_uiSysPort, SERVER_CON_SEC, MAX_SIZE_512M) == 0);

	m_pGameNetface = new CGameNetface();
	//gateid和groupid相同(一个网管对应一个服务器)
	m_pGameNetface->SetServerID(m_oConstConfig.m_uiGroupId);
	ERROR_EXIT(m_pLogic->add_reconn(m_pGameNetface, m_oConstConfig.m_strGameIp, m_oConstConfig.m_uiGamePort, SERVER_CON_SEC, MAX_SIZE_512M) == 0);

	//注册read回调和write回调
	regfn_io_recv_msg(my_io_recv_msg);
	regfn_io_send_msg(my_io_send_msg);

	//初始化定时任务
	m_pUserUpEvent = new CTimeEvent(m_pLogic->evthread()->Base(), true, 100, std::bind(&CUserManager::OnTimer, &m_oUserMgr, std::placeholders::_1));
	if (m_pUserUpEvent == nullptr || !m_pUserUpEvent->init())
	{
		Log_Error("init user up time event error !!!");
		return false;
	}

	m_pEvent = new CTimeEvent(m_pLogic->evthread()->Base(), true, 1000, std::bind(&CGateLogic::OnTimer, this, std::placeholders::_1));
	if (m_pEvent == nullptr || !m_pEvent->init())
	{
		Log_Error("init time event error !!!");
		return false;
	}
	return true;
}

bool CGateLogic::Run()
{
	WritePidFile();
	if (m_pLogic) {
		m_pLogic->run();
		return true;
	}
	return false;
}

void CGateLogic::Stop()
{
	if (m_pUserUpEvent != nullptr) 
		m_pUserUpEvent->stop();
	if (m_pLogic != nullptr)
		m_pLogic->stop();
}

void CGateLogic::Fini()
{
	std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	SAFE_DELETE(m_pUserUpEvent);
	SAFE_DELETE(m_pClientLIF);
	if (m_pLogic)
	{
		CLogicThread::del(m_pLogic);
		m_pLogic = NULL;
	}
	GetLogMgrInstance()->UnInit();
}

void CGateLogic::OnTimer(uint32 dwTm)
{
	//Log_Custom("start", "OnTime %d.......", dwTm);
}

bool CGateLogic::SetMaxSend(uint32 uiSize) 
{ 
	if (uiSize > m_uiMaxSend)
	{
		m_uiMaxSend = uiSize;
		return true;
	}
	return false;
}

void CGateLogic::SendWChatMsg(const std::string& strClient
	, const std::string& strServer, const std::string& strMsg)
{
	m_pSysNetface->SendWChatMsg(strClient, strServer, strMsg);
}
