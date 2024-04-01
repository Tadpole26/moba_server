#pragma once
#include "CTcpUdpLogic.h"
#include "svr_client.h"

class CTcpSess : public svr_session, public CTcpUdpSession
{
public:
	CTcpSess();
	~CTcpSess();

	virtual bool Send_Msg(google::protobuf::Message* pMsg, int iCmd
		, int iModule = MsgModule::Battle, ResultCode eCode = Code_Common_Success
		, int iSeqid = 0);

	virtual bool Send_Msg(const std::string& strMsg, int iCmd
		, int iModule = MsgModule::Battle, ResultCode eCode = Code_Common_Success
		, int iSeqid = 0);

	virtual bool Send_Msg(const uchar* pBuf, size_t uiSize, int iCmd
		, int iModule = MsgModule::Battle, ResultCode eCode = Code_Common_Success
		, int iSeqid = 0);

	virtual bool Send(const tagMsgHead* pMsg);

	virtual bool IsTcp() { return true; }
	virtual bool InitTcpUdp(int64_t llCreate);
	virtual void Tick(int64_t llTick);
	virtual bool IsOut();

	bool IsSameConn(const tagHostHd& stHead);
	void handle_msg(const tagMsgHead* pMsg);

	virtual void on_disconnect();
};
