#pragma once
#include <string>

//战斗地图信息
struct battle_map_info_t
{
	int m_iMapId = 0;						//地图编号
	int m_iMapType = 0;						//类型对应房间类型
	std::string m_strMapName;				//地图名字
	int m_iWeight = 0;						//随机权重
	int m_iSecond = 300;					//战斗时长

	void Clear()
	{
		m_iMapId = 0;
		m_iMapType = 0;
		m_strMapName.clear();
		m_iWeight = 0;
		m_iSecond = 0;
	}
};