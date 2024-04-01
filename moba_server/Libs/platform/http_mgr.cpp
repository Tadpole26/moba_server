#include "pch.h"
#include "http_mgr.h"
#include "log_mgr.h"
#include "util_time.h"

CHttpMgr::CHttpMgr()
{
}

CHttpMgr::~CHttpMgr()
{
	_bStop = true;
	for (auto iter = _vecThread.begin(); iter != _vecThread.end(); ++iter)
	{
		if (iter->joinable())
			iter->join();
	}
	_vecThread.clear();
	curl_global_cleanup();
}

bool CHttpMgr::Init(size_t uiMaxThread)
{
	if (CURLE_OK != curl_global_init(CURL_GLOBAL_ALL))
		return false;

	if (uiMaxThread < 1) uiMaxThread = 1;

	_uiThreadNum = uiMaxThread;
	_vecThread.resize(uiMaxThread);

	for (size_t i = 0; i < _uiThreadNum; ++i)
	{
		_vecThread[i] = std::thread(std::bind(CHttpMgr::ThreadFunc, this));
	}
	return true;
}

void CHttpMgr::RegFunc(HttpResFunc func)
{
	_func = func;
}

void CHttpMgr::PushPost(CHttpParam& data)
{
	CHttpParam* pParam = new CHttpParam(data);
	if (pParam == nullptr)
	{
		Log_Error("new error:%s", data.GetUrl().c_str());
		return;
	}
	_lock.lock();
	_list.push_back(pParam);
	_lock.unlock();
}

void CHttpMgr::ThreadFunc(CHttpMgr* pThis)
{
	CHttpClient htpClt;
	CHttpParam* pItem = nullptr;
	std::string strRes, strUrl, strFields;
	int32_t iPreErr = 0;
	uint32 uiSleep = 0;
	SecTimer<120> tmrLog;
	uint32 uiItemSize = 0;
	while (!pThis->_bStop)
	{
		pThis->_lock.lock();
		if (pThis->_list.size() == 0)
		{
			uiSleep = (uiSleep + 1) % 10 + 1;
			pThis->_lock.unlock();
			//每隔120s打条日志
			if (tmrLog.On(GetCurrTime()))
			{
				Log_Custom("http", "interval:%u, size:0", tmrLog._interval, pThis->_list.size());
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(uiSleep));
			continue;
		}
		else
		{
			uiItemSize = (uint32)pThis->_list.size();
			uiSleep = 0;
			pItem = pThis->_list.front();
			pThis->_list.pop_front();
			pThis->_lock.unlock();

			if (tmrLog.On(GetCurrTime()))
				Log_Custom("http", "size:%d", uiItemSize);

			strUrl = pItem->GetUrl();
			strFields = pItem->GetFields();
			if (!strUrl.empty())
			{
				strRes.clear();
				int32_t uiRc = 0;
				for (uint8_t i = 0; i < 60; ++i)
				{
					try {
						if (pItem->m_bPost)
							uiRc = htpClt.Post(strUrl.c_str(), pItem->GetMapFields(), pItem->GetFields(), strRes);
						else
							uiRc = htpClt.Get(strUrl.c_str(), strRes);

						if (uiRc == CURLE_OK)
						{
							iPreErr = 0;
							break;
						}
						else
						{
							Log_Custom("httpError", "Error: %u url:%s", uiRc, strUrl.c_str());
							if (iPreErr >= 5)
								break;
						}
					}
					catch (std::exception& exp)
					{
						Log_Custom("httpError", "Error: %u url:%s err:%s", uiRc, strUrl.c_str(), exp.what());
					}
					catch (...)
					{
						Log_Custom("httpError", "Error: %u url:%s", uiRc, strUrl.c_str());
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
				pItem->m_strRet = strRes;
				pItem->m_iCode = uiRc;

				if (pItem->m_bRet && pThis->_func != nullptr)
					pThis->_func(*pItem);

				if (uiRc != CURLE_OK)
					++iPreErr;

				delete pItem;
				pItem = nullptr;
			}
		}
	}
}