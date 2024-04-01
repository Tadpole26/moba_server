#pragma once
#include "CProcessorBase.h"
#include "msg_module_login.pb.h"
#include "parse_pb.h"

class CLoginProcessor : public CProcessorBase
{
public:
	CLoginProcessor(uint32_t uiModuleId): CProcessorBase(uiModuleId) { }


protected:
	bool DoUserRun(const std::string& strMsg) override;

	int GetItemList(const std::string& strMsg, Msg_Login_GetItemList_Rsp& oItemRsp);

	int AwardItem(const std::string& strMsg, Msg_Login_AwardItem_Rsp& oAwardRsp);
};

