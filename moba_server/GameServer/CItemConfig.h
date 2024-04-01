#pragma once
#include "util_load_base.h"
#include "msg_common.pb.h"
using namespace ProtoMsg;

enum eItemType
{
	eItemType_None = 0,							
	eItemType_Currency = 1,						//货币类
	eItemType_UserExp = 2,						//玩家经验
	eItemType_VipExp = 3,						//vip经验
	eItemType_HeroSoul = 4,						//英雄碎片
};

enum eItemId
{
	eItemId_None = 0,
	eItemId_Gold = 1,							//金币
	eItemId_Diamond = 2,						//钻石
	eItemId_Power = 3,							//体力
	eItemId_Soul = 4,							//万能碎片
	eItemId_UserExp = 1001,						//玩家经验
	eItemId_VipExp = 1002,						//Vip经验
};

struct item_elem_t
{
	uint32_t					m_uiItemId = 0;
	uint32_t					m_uiItemType = 0;
};


class CItemConfig : public CUtilLoadBase
{
public:
	bool LoadConfig(const std::string& strContent) override;

	item_elem_t* GetItemElemCfg(uint32_t uiItemId);

private:
	bool AddItemInfo();

private:
	std::map<uint32_t, item_elem_t> m_mapItemInfoCfg;
};

#define ITEM_CFG_INS Singleton<CItemConfig>::getInstance()
