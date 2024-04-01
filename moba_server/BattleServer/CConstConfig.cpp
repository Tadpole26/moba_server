#include "pch.h"
#include "CConstConfig.h"
#include "global_define.h"
#include "util_file.h"
#include "log_mgr.h"

bool CConstConfig::Init()
{
	InitSheet("common_config", std::bind(&CConstConfig::AddCommonCfg, this));
	InitSheet("cross_config", std::bind(&CConstConfig::AddCrossCfg, this));
	InitSheet("battle_config", std::bind(&CConstConfig::AddBattleCfg, this));

	return LoadCfgFromXml(CONST_CONFIG);
}

bool CConstConfig::AddCommonCfg()
{
	m_iGroupId = GetDataInt("group");
	m_iPlatId = GetDataInt("platid");
	return true;
}

bool CConstConfig::AddCrossCfg()
{
	m_strBCenterIp = GetDataChr("ip");
	m_iBCenterPort = GetDataInt("port");
	return true;
}

bool CConstConfig::AddBattleCfg()
{
	m_iMaxUser = GetDataInt("max_user");
	m_bUdp = GetDataBool("udp");
	m_iTimeOut = GetDataInt("timeout");
	m_iMaxThread = GetDataInt("max_thread");
	m_strBattleIp = GetDataChr("ip");
	m_iBattlePort = GetDataInt("port");
	return true;
}

