#pragma once
#include <string>
#include "util_load_base.h"
//��ȡ����zk����(ip port�ȵ�)
class CConstConfig : public CUtilLoadBase
{
public:
	bool Init();

	bool AddCommonCfg();
public:
	int32_t									m_iPlatId = 0;
	int32_t									m_iGroupId = 0;

	//BCenter server
	std::string								m_strBCenterIp = "";
	int										m_iBCenterPort = 0;

	std::string								m_strGameApiUrl = "";

};
