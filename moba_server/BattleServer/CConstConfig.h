#pragma once
#include <string>
#include "util_load_base.h"
//读取本地zk配置(ip port等等)
class CConstConfig : public CUtilLoadBase
{
public:
	bool Init();

	bool AddCommonCfg();
	bool AddCrossCfg();
	bool AddBattleCfg();

	bool IsTcp() { return !m_bUdp; }
public:
	int32_t									m_iPlatId = 0;
	int32_t									m_iGroupId = 0;

	int										m_iMaxUser = 100;
	bool									m_bUdp = false;
	int										m_iTimeOut = 0;
	int										m_iMaxThread = 0;			//启动线程数量

	//BCenter server
	std::string								m_strBCenterIp = "";
	int										m_iBCenterPort = 0;

	//log server
	std::string								m_strLogIp = "";
	int										m_iLogPort = 0;

	//battle server
	std::string								m_strBattleIp = "";	
	int										m_iBattlePort = 0;					
	int										m_iBattleId = 0;

};
