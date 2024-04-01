#pragma once

//游戏道具结构
struct game_item_t
{
	MSGPACK_DEFINE(m_iItemType, m_iCardID, m_iNum);

	rk_logic_general_game_item_pack_t(int iItemType = 0, int iCardID = 0, int iNum = 0) :
		m_iItemType(iItemType), m_iCardID(iCardID), m_iNum(iNum) {}

	int                     m_iItemType;
	int                     m_iCardID;
	int                     m_iNum;
};
