#include "CBattleLogic.h"
#include "CUdpLogic.h"
#include "log_mgr.h"
#include "util_file.h"
#include "coredump_x.h"
#include "func_proc.h"
#include "global_define.h"
#include "CRoomManager.h"
#include "CUserManager.h"

CBattleLogic::CBattleLogic() 
{
}

CBattleLogic::~CBattleLogic()
{
}

int CBattleLogic::GetSvrId()
{
	static int iSvrId = 0;
	if (iSvrId != 0)
		return iSvrId;

	iSvrId = GetGroupId() * 100 + GetIndex();

	return iSvrId;
}

bool CBattleLogic::Init()
{
	CUdpLogic::InitKcp();

#ifdef _WIN32
	Log_Custom("start", "exe: %s ...", get_exec_name().c_str());
#else 
	Log_Custom("start", "exe: %s, index:%d, version:%s begin...", get_exec_name().c_str(), GetIndex(), Version().c_str());
#endif
	//设置coredump文件和core回调
	InstallCoreDumper();
	SetConsoleInfo(std::bind(&CBattleLogic::Stop, this));

	//读取本地配置
	if (!m_oBattleConfig.Init())
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
	if (!GetLogMgrInstance()->Init(m_oBattleConfig.m_iGroupId, m_stArgOpt.GetIndex()))
	{
		Log_Error("log manager init error!!!");
		return false;
	}

	if (is_listen_port(m_oBattleConfig.m_iBattlePort))
	{
		Log_Error("listen port exsit %d!!!", m_oBattleConfig.m_iBattlePort);
		return false;
	}

	net_setting stSetting;
	stSetting.m_reThread.Init(1, 64, ACCEPT_BUF_SIZE, SERVER_BUF_SIZE, SERVER_BUF_SIZE);
	stSetting.InitMq(EMQ_NO);

	m_pLogic = CLogicThread::newm(stSetting);
	if (m_pLogic == nullptr)
	{
		Log_Error("logic thread new failed!");
		return false;
	}
	CSvrLogicFace* pReconnLIF = new CSvrLogicFace();
	if (pReconnLIF == nullptr)
		return false;
	m_pLogic->reg_interface_reconn(pReconnLIF);

	//连接BCenter server
	m_pCrossNetFace = new CCrossNetFace();
	if (m_pCrossNetFace == nullptr)
		return false;
	m_pCrossNetFace->SetServerID(GetSvrId());
	if (m_pLogic->add_reconn(m_pCrossNetFace, m_oBattleConfig.m_strBCenterIp.c_str(), m_oBattleConfig.m_iBCenterPort,
		SERVER_CON_SEC, MAX_SIZE_512M) < 0)
		return false;
	//连接Log server
	m_pLogNetface = new CLogNetFace();
	if (m_pLogNetface == nullptr)
		return false;
	m_pLogNetface->SetServerID(GetSvrId());
	if (m_pLogic->add_reconn(m_pLogNetface, m_oBattleConfig.m_strLogIp.c_str(), m_oBattleConfig.m_iLogPort,
		SERVER_CON_SEC, MAX_SIZE_512M) < 0)
		return false;

	regfn_io_recv_msg(my_io_recv_msg);
	regfn_io_send_msg(my_io_send_msg);

	m_pSvrEvent = new CTimeEvent(m_pLogic->evthread()->Base(), true, 100
				, std::bind(&CBattleLogic::OnTimer, this, std::placeholders::_1));
	if (!m_pSvrEvent->init())
		return false;

	if (!gRoomManager->Init(m_oBattleConfig.m_iBattlePort, m_oBattleConfig.m_iMaxThread, m_oBattleConfig.IsTcp()))
	{
		Log_Error("CRoomManager init error!!");
		return false;
	}
	return true;
}

bool CBattleLogic::Run()
{
	if (!m_pLogic)
		return false;
	m_pLogic->run();
	return true;
}

void CBattleLogic::Fini()
{
	SAFE_DELETE(m_pCrossNetFace);
	SAFE_DELETE(m_pLogNetface);
	SAFE_DELETE(m_pSvrEvent);

	CLogicThread::del(m_pLogic);
}

void CBattleLogic::Stop()
{
	gRoomManager->Stop();
	if (m_pSvrEvent)
		m_pSvrEvent->stop();
	if (m_pLogic != nullptr)
		m_pSvrEvent->stop();
}

void CBattleLogic::OnTimer(int iTime)
{
	SetCurrTime();
	gRoomManager->OnTimer(iTime);
	gUserManager->OnTimer(iTime);
}

bool CBattleLogic::Arg(int argc, char* argv[])
{
	return m_stArgOpt.Argv(argc, argv);
}

bool CBattleLogic::Launch()
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
