#pragma once
#include "util_load_base.h"
#include "msg_common.pb.h"
using namespace ProtoMsg;

struct hero_info_t
{
	int							m_iHeroId = 0;
	std::string					m_strHeroName = "";
};

struct hero_up_level_t
{
	int							m_iHeroLevel = 0;
	std::vector<game_item_t>	m_vecCost;
};

class CHeroConfig : public CUtilLoadBase
{
public:
	bool LoadConfig(const std::string& strContent) override;

private:
	bool AddHeroInfo();
	bool AddHeroUpLevel();

private:
	std::map<uint32_t, hero_info_t> m_mapHeroInfoCfg;
	std::map<uint32_t, hero_up_level_t> m_mapHeroUpCfg;
};

#define HERO_CFG_INS Singleton<CHeroConfig>::getInstance()
