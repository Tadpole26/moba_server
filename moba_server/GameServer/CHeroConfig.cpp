#include "CHeroConfig.h"
#include "log_mgr.h"

bool CHeroConfig::LoadConfig(const std::string& strContent)
{
	InitSheet("hero_info", std::bind(&CHeroConfig::AddHeroInfo, this));
	InitSheet("hero_up_level", std::bind(&CHeroConfig::AddHeroUpLevel, this));

	return LoadCfgFromStr(strContent);
}

bool CHeroConfig::AddHeroInfo()
{
	uint32_t uiHeroId = GetDataUInt("id");
	hero_info_t& stCfg = m_mapHeroInfoCfg[uiHeroId];
	stCfg.m_iHeroId = uiHeroId;
	stCfg.m_strHeroName = GetDataChr("name");
	return true;
}

bool CHeroConfig::AddHeroUpLevel()
{
	uint32_t uiHeroLevel = GetDataUInt("level");
	hero_up_level_t& stCfg = m_mapHeroUpCfg[uiHeroLevel];
	stCfg.m_iHeroLevel = uiHeroLevel;
	std::string strCost = GetDataChr("cost");
	//if (!CCommonItem::ParseItem(strCost, stCfg.m_vecCost))
	//{
	//	Log_Error("load cost error, cost:%s", strCost.c_str());
	//	return false;
	//}
	return true;
}