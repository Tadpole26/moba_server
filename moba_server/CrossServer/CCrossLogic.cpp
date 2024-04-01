#include "CCrossLogic.h"
#include "global_define.h"
#include "log_mgr.h"
#include "func_proc.h"
#include "global_define.h"
#include "coredump_x.h"
#include "util_file.h"
#include <fstream>

bool CCrossLogic::Arg(int argc, char* argv[])
{
	return m_stArgOpt.Argv(argc, argv);
}

void CCrossLogic::WritePidFile()
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

bool CCrossLogic::Init()
{
#ifdef _WIN32
	Log_Custom("start", "exe: %s ...", get_exec_name().c_str());
#else 
	Log_Custom("start", "exe: %s, index:%d, version:%s begin...", get_exec_name().c_str(), GetIndex(), Version().c_str());
#endif
	//设置coredump文件和core回调
	InstallCoreDumper();
	SetConsoleInfo(std::bind(&CCrossLogic::Stop, this));

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

	if (is_listen_port(m_oConstCfg.m_uiCrossPort))
	{
		Log_Error("listen port exsit %u!!!", m_oConstCfg.m_uiCrossPort);
		return false;
	}

	net_setting stNetConfig;
	stNetConfig.m_nListenPort = m_oConstCfg.m_uiCrossPort;
	//线程数量,每个线程可连接数量,接受,输入,输出包大小限制
	stNetConfig.m_ioThread.Init(get_cpu_num(), 4096, ACCEPT_BUF_SIZE, SERVER_BUF_SIZE, SERVER_BUF_SIZE);
	stNetConfig.m_reThread.Init(1, 10, ACCEPT_BUF_SIZE, SERVER_BUF_SIZE, SERVER_BUF_SIZE);

	m_pLogic = CLogicThread::newm(stNetConfig);
	if (m_pLogic == nullptr)
	{
		Log_Error("logic thread new failed!");
		return false;
	}

	m_pClientLIF = new CClientLogic();
	assert(m_pClientLIF);
	m_pLogic->reg_interface_listen(m_pClientLIF);

	m_pReconInterface = new CSvrLogicFace();
	assert(m_pReconInterface);
	m_pLogic->reg_interface_reconn(m_pReconInterface);

	regfn_io_recv_msg(my_io_recv_msg);
	regfn_io_send_msg(my_io_send_msg);
	
	Log_Info("success platid:%u, groupid:%u", m_oConstCfg.m_uiPlatId, m_oConstCfg.m_uiGroupId);
	return true;
}

bool CCrossLogic::Run()
{
	WritePidFile();
	if (m_pLogic)
	{
		m_pLogic->run();
		return true;
	}
	return false;
}

void CCrossLogic::Stop()
{
	if (m_pLogic != nullptr)
		m_pLogic->stop();
}

void CCrossLogic::Fini()
{
	if (m_pLogic)
	{
		CLogicThread::del(m_pLogic);
		m_pLogic = nullptr;
	}
}

bool CCrossLogic::Launch()
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