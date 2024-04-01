#pragma once
#include <string>

//ս����ͼ��Ϣ
struct battle_map_info_t
{
	int m_iMapId = 0;						//��ͼ���
	int m_iMapType = 0;						//���Ͷ�Ӧ��������
	std::string m_strMapName;				//��ͼ����
	int m_iWeight = 0;						//���Ȩ��
	int m_iSecond = 300;					//ս��ʱ��

	void Clear()
	{
		m_iMapId = 0;
		m_iMapType = 0;
		m_strMapName.clear();
		m_iWeight = 0;
		m_iSecond = 0;
	}
};