#pragma once
#include "util_load_base.h"
#include "msg_common.pb.h"
#include "singleton.h"
using namespace ProtoMsg;


struct user_level_elem_t
{
	uint32_t uiLevel = 0;
	uint32_t uiExp = 0;
	std::vector<game_item_t> vecReward;
};

struct user_vip_elem_t
{
	uint32_t uiVip = 0;
	uint32_t uiExp = 0;
	std::vector<game_item_t> vecReward;
};


class CUserConfig : public CUtilLoadBase
{
public:
	bool LoadConfig(const std::string& strContent) override;

	std::map<uint32_t, user_level_elem_t>& GetUserLevelMapCfg();
	user_level_elem_t* GetUserLevelElemCfg(uint32_t uiLevel);
	std::map<uint32_t, user_vip_elem_t>& GetUserVipMapCfg();
	user_vip_elem_t* GetUserVipElemCfg(uint32_t uiVip);
private:
	bool AddUserUpLevel();
	bool AddUserUpVip();

private:
	std::map<uint32_t, user_level_elem_t> m_mapUserLevelCfg;
	std::map<uint32_t, user_vip_elem_t> m_mapUserVipCfg;
};

#define USER_CFG_INS Singleton<CUserConfig>::getInstance()
