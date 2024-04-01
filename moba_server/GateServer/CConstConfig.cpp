#include "CConstConfig.h"
#include "global_define.h"
#include "util_file.h"
#include "tinystr.h"
#include "tinyxml.h"
#include "log_mgr.h"

bool CConstConfig::Init()
{
	InitSheet("common_config", std::bind(&CConstConfig::AddCommonCfg, this));
	InitSheet("gate_config", std::bind(&CConstConfig::AddGateCfg, this));
	InitSheet("game_config", std::bind(&CConstConfig::AddGameCfg, this));
	InitSheet("sys_config", std::bind(&CConstConfig::AddSysCfg, this));

	return LoadCfgFromXml(CONST_CONFIG);
}

bool CConstConfig::AddCommonCfg()
{
	m_uiGroupId = GetDataUInt("group");
	m_uiPlatId = GetDataUInt("platid");
	return true;
}

bool CConstConfig::AddGateCfg()
{
	m_strGateIp = GetDataChr("ip");
	m_uiGatePort = GetDataUInt("port");
	m_uiLimit = GetDataUInt("limit");
	m_uiCheckMax = GetDataUInt("check_max");
	m_uiTimeout = GetDataUInt("timeout");
	m_uiWaitCheck = GetDataUInt("wait_check");
	m_uiNotifyInterval = GetDataUInt("notify_interval");
	return true;
}

bool CConstConfig::AddGameCfg()
{
	m_strGameIp = GetDataChr("ip");
	m_uiGamePort = GetDataUInt("port");
	return true;
}

bool CConstConfig::AddSysCfg()
{
	m_strSysIp = GetDataChr("ip");
	m_uiSysPort = GetDataUInt("port");
	return true;
}