#include "CItemConfig.h"

bool CItemConfig::LoadConfig(const std::string& strContent)
{
	InitSheet("item_info", std::bind(&CItemConfig::AddItemInfo, this));

	return LoadCfgFromStr(strContent);
}

item_elem_t* CItemConfig::GetItemElemCfg(uint32_t uiItemId)
{
	auto iter = m_mapItemInfoCfg.find(uiItemId);
	if (iter == m_mapItemInfoCfg.end())
		return nullptr;

	return &(iter->second);
}

bool CItemConfig::AddItemInfo()
{
	uint32_t uiItemId = GetDataUInt("item_id");
	item_elem_t& stCfg = m_mapItemInfoCfg[uiItemId];
	stCfg.m_uiItemId = uiItemId;
	stCfg.m_uiItemType = GetDataUInt("item_type");
	return true;
}



