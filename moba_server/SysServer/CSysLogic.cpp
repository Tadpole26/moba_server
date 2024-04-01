#include "CSysLogic.h"
#include "global_define.h"
#include "func_proc.h"
#include "SysHttpFunc.h"
#include "util_time.h"
#include "coredump_x.h"
#include <fstream>
#include "util_file.h"

CSysLogic::CSysLogic()
{
	m_strExIp = get_net_ip();
}

CSysLogic::~CSysLogic()
{
}

bool CSysLogic::Arg(int argc, char* argv[])
{
	return m_stArgOpt.Argv(argc, argv);
}

void CSysLogic::WritePidFile()
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

bool CSysLogic::Init()
{
#ifdef _WIN32
	Log_Custom("start", "exe: %s ...", get_exec_name().c_str());
#else 
	Log_Custom("start", "exe: %s begin...", get_exec_name().c_str());
#endif
	//设置coredump文件和core回调
	InstallCoreDumper();
	SetConsoleInfo(std::bind(&CSysLogic::Stop, this));

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

	//初始化网络配置
	net_setting stSetting;
	stSetting.m_nListenPort = m_oConstCfg.m_uiSysPort;
	stSetting.m_ioThread.Init(4, MAX_CON_SERVER, ACCEPT_BUF_SIZE
		, SERVER_BUF_SIZE, SERVER_BUF_SIZE);
	if (is_listen_port(stSetting.m_nListenPort))
	{
		Log_Error("listen port exist %u", stSetting.m_nListenPort);
		return false;
	}

	m_pLogic = CLogicThread::newm(stSetting);
	if (m_pLogic == nullptr)
	{
		Log_Error("logic thread new failed!");
		return false;
	}

	m_pClientLIF = new CClientLogic();
	assert(m_pClientLIF);
	m_pLogic->reg_interface_listen(m_pClientLIF);

	if (!SYS_HTTP_INS->Init(m_oConstCfg.m_strApiAddr, m_pLogic->evthread()->Base()))
	{
		Log_Error(" http init error! url:%s", m_oConstCfg.m_strApiAddr.c_str());
		return false;
	}

	regfn_io_recv_msg(my_io_recv_msg);
	regfn_io_send_msg(my_io_send_msg);
	Log_Info("init sys server succeed!");
	return true;
}

bool CSysLogic::Run()
{
	if (m_pLogic)
	{
		m_pLogic->run();
		return true;
	}
	return false;
}

void CSysLogic::Fini()
{
	SAFE_DELETE(m_pClientLIF);
	
	if (m_pLogic)
	{
		CLogicThread::del(m_pLogic);
		m_pLogic = NULL;
	}
}

void CSysLogic::Stop()
{
	if (m_pLogic != nullptr)
		m_pLogic->stop();
}

void CSysLogic::OnTimer(uint32 dwTm)
{
	SetCurrTime();
	SYS_HTTP_INS->OnTimer();
}

bool CSysLogic::Launch()
{
	if (!Init())
	{
		Stop();
		Fini();
		return false;
	}
	std::string strLocalIp = get_local_ip();
	Log_Custom("start", "local_id:%s", strLocalIp.c_str());
	bool bRet = Run();
	Fini();
	return bRet;
}
