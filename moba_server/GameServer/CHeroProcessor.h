#pragma once
#include "CProcessorBase.h"
#include "msg_module_hero.pb.h"

class CHeroProcessor : public CProcessorBase
{
public:
	CHeroProcessor(uint32_t uiModuleId) : CProcessorBase(uiModuleId) { }


protected:
	bool DoUserRun(const std::string& strMsg) override;

	int UpHeroLevel(const std::string& strMsg, Msg_Hero_UpHeroLevel_Rsp& oLevelRsp);

	int UpHeroStar(const std::string& strMsg, Msg_Hero_UpHeroStar_Rsp& oStarRsp);
};
