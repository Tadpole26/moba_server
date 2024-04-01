#pragma once
#include <string>
#include "util_load_base.h"

//���ط���������
class CConstConfig : public CUtilLoadBase
{
public:
public:
	bool Init();

	bool AddCommonCfg();
	bool AddGateCfg();
	bool AddGameCfg();
	bool AddSysCfg();
public:
	uint32_t					m_uiGroupId = 0;//����id
	uint32_t					m_uiPlatId = 0;//ƽ̨id

	std::string					m_strGateIp = "";
	uint32_t 					m_uiGatePort = 0;
	uint32_t					m_uiLimit = 0;		//�������Ϸ�������ֵ
	uint32_t					m_uiCheckMax = 0;	//ÿ����֤����������
	uint32_t					m_uiTimeout = 0;
	uint32_t					m_uiWaitCheck = 0;
	uint32_t					m_uiNotifyInterval = 0;

	std::string					m_strGameIp = "";
	uint32_t					m_uiGamePort = 0;

	std::string					m_strSysIp = "";
	uint32_t					m_uiSysPort = 0;
};
