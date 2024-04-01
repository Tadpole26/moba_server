#include "CUserConfig.h"
#include "log_mgr.h"

bool CUserConfig::LoadConfig(const std::string& strContent)
{
	InitSheet("user_up_level", std::bind(&CUserConfig::AddUserUpLevel, this));
	InitSheet("user_up_vip", std::bind(&CUserConfig::AddUserUpVip, this));

	return LoadCfgFromStr(strContent);
	
}

std::map<uint32_t, user_level_elem_t>& CUserConfig::GetUserLevelMapCfg()
{
	return m_mapUserLevelCfg;
}

user_level_elem_t* CUserConfig::GetUserLevelElemCfg(uint32_t uiLevel)
{
	auto iter = m_mapUserLevelCfg.find(uiLevel);
	if (iter == m_mapUserLevelCfg.end())
		return nullptr;
	return &(iter->second);
}

std::map<uint32_t, user_vip_elem_t>& CUserConfig::GetUserVipMapCfg()
{
	return m_mapUserVipCfg;
}

user_vip_elem_t* CUserConfig::GetUserVipElemCfg(uint32_t uiVip)
{
	auto iter = m_mapUserVipCfg.find(uiVip);
	if (iter == m_mapUserVipCfg.end())
		return nullptr;
	return &(iter->second);
}

bool CUserConfig::AddUserUpLevel()
{
	uint32_t uiUserLevel = GetDataUInt("level");
	user_level_elem_t& stElem = m_mapUserLevelCfg[uiUserLevel];
	stElem.uiLevel = uiUserLevel;
	stElem.uiExp = GetDataUInt("cost");
	std::string strReward = GetDataChr("reward");
	//if (!CCommonItem::ParseItem(strReward, stElem.vecReward))
	//{
	//	Log_Error("load reward error, reward:%s", strReward.c_str());
	//	return false;
	//}
	return true;
}

bool CUserConfig::AddUserUpVip()
{
	uint32_t uiUserVip = GetDataUInt("vip");
	user_vip_elem_t& stElem = m_mapUserVipCfg[uiUserVip];
	stElem.uiVip = uiUserVip;
	stElem.uiExp = GetDataUInt("cost");
	std::string strReward = GetDataChr("reward");
	//if (!CCommonItem::ParseItem(strReward, stElem.vecReward))
	//{
	//	Log_Error("load reward error, reward:%s", strReward.c_str());
	//	return false;
	//}
	return true;
}