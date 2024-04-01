#pragma once
#include "util_load_base.h"
#include "msg_common.pb.h"
using namespace ProtoMsg;

enum eItemType
{
	eItemType_None = 0,							
	eItemType_Currency = 1,						//������
	eItemType_UserExp = 2,						//��Ҿ���
	eItemType_VipExp = 3,						//vip����
	eItemType_HeroSoul = 4,						//Ӣ����Ƭ
};

enum eItemId
{
	eItemId_None = 0,
	eItemId_Gold = 1,							//���
	eItemId_Diamond = 2,						//��ʯ
	eItemId_Power = 3,							//����
	eItemId_Soul = 4,							//������Ƭ
	eItemId_UserExp = 1001,						//��Ҿ���
	eItemId_VipExp = 1002,						//Vip����
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
