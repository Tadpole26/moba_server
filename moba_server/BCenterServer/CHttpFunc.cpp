#include "CHttpFunc.h"
#include "http_mgr.h"
#include "log_mgr.h"

CHttpMgr gHttpMgr;
CHttpFunc::CHttpFunc()
{
}

CHttpFunc::~CHttpFunc()
{
}

void CHttpFunc::Run(CHttpParam& oParam)
{
	gHttpMgr.PushPost(oParam);
}

bool CHttpFunc::Init(std::string strHost, int iThreadNum)
{
	bool bRet = gHttpMgr.Init(iThreadNum);
	if (!bRet)
	{
		Log_Error("http mgr init error!");
		return false;
	}
	CHttpRequest::Init(strHost);
	return true;
}