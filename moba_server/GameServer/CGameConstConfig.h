#pragma once
#include "util_load_base.h"
#include "msg_common.pb.h"

class CGameConstConfig : public CUtilLoadBase
{
public:
	bool LoadConfig(const std::string& strContent) override;

	int GetMailCountMax() const { return m_iMailCountMax; }

private:
	bool AddConstInfo();

private:
	int m_iMailCountMax = 0;
};

#define GAME_CONST_CFG_INS Singleton<CGameConstConfig>::getInstance()
