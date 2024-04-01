#pragma once
#include "svr_client.h"

class CRoom;
class CCrossNetFace : public svr_reconn
{
public:
	CCrossNetFace(void);
	~CCrossNetFace(void);

	void on_connect();
	void on_disconnect();
	virtual void trigger();

	void handle_msg(const tagMsgHead* pNetMsg);
	//const std::string& GetSvrIdStr();

private:
	//void SubmintReport();
	//void SubmintRoomInfo(CRoom* pRoom);

	//void OnCreateRoom(uchar* pMsg, int iSize);

	std::string m_strSvrId;
};
