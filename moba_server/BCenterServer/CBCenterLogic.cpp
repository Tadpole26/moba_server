#include "CBCenterLogic.h"
#include "global_define.h"
#include "log_mgr.h"
#include "func_proc.h"
#include "global_define.h"
#include "coredump_x.h"
#include "util_file.h"
#include "mongo_base.h"
#include "CRoomManager.h"
#include "CBattleManager.h"
#include "CHttpFunc.h"
#include <fstream>

bool CBCenterLogic::Arg(int argc, char* argv[])
{
	return m_stArgOpt.Argv(argc, argv);
}

void CBCenterLogic::WritePidFile()
{
	std::string strFilePath;
#ifdef _WIN32
	strFilePath = get_exec_path() + "/tmp/";
#else 
	strFilePath = "/tmp/gundam/";
#endif
	create_dir(strFilePath.c_str());
	strFilePath += "_";
	strFilePath += std::to_string(m_oConstCfg.m_iBCenterPort);
	strFilePath += ".pid";

	std::ofstream _ofs;
	_ofs.open(strFilePath.c_str(), std::ios_base::trunc | std::ios_base::out);
	_ofs << get_pid() << std::endl;
	_ofs.close();
}

bool CBCenterLogic::Init()
{
#ifdef _WIN32
	Log_Custom("start", "exe: %s ...", get_exec_name().c_str());
#else 
	Log_Custom("start", "exe: %s begin...", get_exec_name().c_str());
#endif
	//设置coredump文件和core回调
	InstallCoreDumper();
	SetConsoleInfo(std::bind(&CBCenterLogic::Stop, this));

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
	if (!GetLogMgrInstance()->Init(m_oConstCfg.m_iGroupId, m_stArgOpt.GetIndex()))
	{
		Log_Error("log init error!!!");
		return false;
	}

	if (!gRoomManager->Init())
	{
		Log_Error("room manager init failed!");
		return false;
	}

	if (!gHttpManager->Init(m_oConstCfg.m_strGameApiUrl))
	{
		Log_Error("http init error! url:%s", m_oConstCfg.m_strGameApiUrl.c_str());
		return false;
	}

	net_setting stNetConfig;
	stNetConfig.m_nListenPort = m_oConstCfg.m_iBCenterPort;
	//线程数量,每个线程可连接数量,接受,输入,输出包大小限制
	stNetConfig.m_ioThread.Init(get_cpu_num(), 4096, ACCEPT_BUF_SIZE, SERVER_BUF_SIZE, SERVER_BUF_SIZE);
	if (is_listen_port(m_oConstCfg.m_iBCenterPort))
	{
		Log_Error("listen port exsit %u!!!", m_oConstCfg.m_iBCenterPort);
		return false;
	}

	m_pLogic = CLogicThread::newm(stNetConfig);
	if (m_pLogic == nullptr)
	{
		Log_Error("logic thread new failed!");
		return false;
	}

	m_pServerLIF = new CClientLogic();
	assert(m_pServerLIF);
	m_pLogic->reg_interface_listen(m_pServerLIF);

	regfn_io_recv_msg(my_io_recv_msg);
	regfn_io_send_msg(my_io_send_msg);

	if (gBattleManager->Init())
	{
		Log_Error("battle manager init failed!");
		return false;
	}

	Log_Info("success platid:%u, groupid:%u", m_oConstCfg.m_iPlatId, m_oConstCfg.m_iGroupId);
	return true;
}

bool CBCenterLogic::Run()
{
	WritePidFile();
	if (m_pLogic)
	{
		m_pLogic->run();
		return true;
	}
	return false;
}

void CBCenterLogic::Stop()
{
	if (m_pLogic != nullptr)
		m_pLogic->stop();
}

void CBCenterLogic::Fini()
{
	SAFE_DELETE(m_pCrossSession);

	if (m_pLogic)
	{
		CLogicThread::del(m_pLogic);
		m_pLogic = nullptr;
	}
}

bool CBCenterLogic::Launch()
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

