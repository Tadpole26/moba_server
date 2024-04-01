#pragma once

#include "logic_interface.h"
#include "msg_module_serverinner.pb.h"
#include "svr_client.h"

class CCrossSession : public svr_session
{
public:
	CCrossSession();
	~CCrossSession();

	virtual void handle_msg(const tagMsgHead* pNetMsg);
	virtual void on_disconnect();

	void HandleMessage(msg_id_t usModule, uint32 uiCmd, const unsigned char* pData, size_t uiLen);
	void HandleCommonModule(uint32 uiCmd, unsigned char* pMsg, size_t uiSize);
	void OnCreateRoomReq(const unsigned char* pData, size_t uiLen);

	int GetPlatId() const { return m_iPlatId; }

private:
	int m_iPlatId = 0;
	int m_iIndex = 0;
};
