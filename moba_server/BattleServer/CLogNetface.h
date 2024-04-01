#pragma once

#include "svr_client.h"

class CLogNetFace : public svr_reconn
{
public:
	CLogNetFace();
	~CLogNetFace();

protected:
	void LogUpLoadFile(int64_t llRoomId, const std::string& strRoomLog, const std::string&strLogPath, size_t uiSize);
	virtual void handle_msg(const tagMsgHead* pNetMsg);
	virtual void on_connect();
	virtual void on_disconnect();
	virtual void trigger();

private:
	void HandleServerCommon(const tagMsgHead* pMsg);
};
