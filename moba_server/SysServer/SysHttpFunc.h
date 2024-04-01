#pragma once
#include "http_request.h"
#include "msg_queue.h"
#include "http_mgr.h"
#include "singleton.h"

class CSysHttpFunc : public CHttpRequest
{
public:
	CSysHttpFunc();
	~CSysHttpFunc();
	bool Init(std::string szHost, event_base* pEvBase, int iThreadNum = 2);

	//非线程安全, 请不要调用
	static void SendMainLogic(uint32_t uiMsgId, const std::string& strMsg);
	static void HttpRetRun(CHttpParam& req);
	static void HttpRetGetUniqRolId(CHttpParam& req);
	static void HttpRetUserLogin(CHttpParam& req);
	static void HttpRetModifyName(CHttpParam& req);

	//主逻辑线程的回调处理
	void OnQueue(const event_msg_t msg);
	void OnRetGetRolId(const std::string& strMsg);
	void OnRetUserLogin(const std::string& strMsg);
	void OnRetModifyName(const std::string& strMsg);

private:
	virtual void Run(CHttpParam& param);
	static CMsgQueue _queue;
};

#define SYS_HTTP_INS Singleton<CSysHttpFunc>::getInstance()
