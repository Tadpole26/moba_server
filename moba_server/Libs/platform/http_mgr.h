#pragma once

#include "http_client.h"
#include "dtype.h"
#include "http_param.h"
#include <list>
#include <functional>
#include <thread>
#include <mutex>
#include <vector>

using namespace std;

using HttpResFunc = std::function<void(CHttpParam& req)>;
class CHttpMgr
{
public:
	CHttpMgr();
	~CHttpMgr();

	bool Init(size_t uiMaxThread = 1);

	void RegFunc(HttpResFunc func);
	void PushPost(CHttpParam& data);
	void Stop() { _bStop = true; }

private:
	static void ThreadFunc(CHttpMgr* pThis);
	bool _bStop = false;

	size_t _uiThreadNum = 1;
	std::vector<std::thread> _vecThread;

	std::list<CHttpParam*> _list;
	mutex _lock;
	HttpResFunc _func;
};
