#pragma once
#include "http_request.h"

class CHttpFunc : public CHttpRequest
{
public:
	CHttpFunc();
	~CHttpFunc();
	bool Init(std::string strHost, int iThreadNum = 2);

private:
	virtual void Run(CHttpParam& oParam);
};

#define gHttpManager Singleton<CHttpFunc>::getInstance()