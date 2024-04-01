#include "pch.h"
#include "CConstConfig.h"
#include "global_define.h"
#include "util_file.h"
#include "log_mgr.h"

bool CConstConfig::Init()
{
	InitSheet("common_config", std::bind(&CConstConfig::AddCommonCfg, this));

	return LoadCfgFromXml(CONST_CONFIG);
}

bool CConstConfig::AddCommonCfg()
{
	m_iGroupId = GetDataInt("group");
	m_iPlatId = GetDataInt("platid");
	return true;
}


