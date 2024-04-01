#include "pch.h"
#include "http_client.h"
#include <iostream>
#include <string>

CHttpClient::CHttpClient()
{
}

CHttpClient::~CHttpClient()
{
}

int32_t CHttpClient::OnDebug(CURL* pURL, curl_infotype iType
	, char* pData, size_t uiSize, void* pVoid)
{
	if (iType == CURLINFO_TEXT)
	{
	}
	else if (iType == CURLINFO_HEADER_IN)
	{
		std::cout << "[HEADER_IN]" << pData << std::endl;
	}
	else if (iType == CURLINFO_HEADER_OUT)
	{
		std::cout << "[HEADER_OUT]" << pData << std::endl;
	}
	else if (iType == CURLINFO_DATA_IN)
	{
		std::cout << "[DATA_IN]" << pData << std::endl;
	}
	else if (iType == CURLINFO_DATA_OUT)
	{
		std::cout << "[DATA_OUT]" << pData << std::endl;
	}
	return 0;
}

size_t CHttpClient::OnWriteData(void* pBuffer, size_t uiSize
	, size_t uiMemB, void* pVoid)
{
	std::string* str = (std::string*)(pVoid);
	if (nullptr == str || nullptr == pBuffer)
		return -1;

	char* pData = (char*)pBuffer;
	str->clear();
	str->append(pData, uiSize * uiMemB);
	return uiMemB;
}


int32_t CHttpClient::Post(const std::string& strUrl, const std::string& strPost, std::string& strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (nullptr == curl)
		return CURLE_FAILED_INIT;

	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int32_t CHttpClient::Post(const std::string& strUrl, std::map<std::string, std::string>& mapFields
	, const std::string& strFile, std::string& strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (nullptr == curl)
		return CURLE_FAILED_INIT;

	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	struct curl_httppost* post = nullptr;
	struct curl_httppost* last = nullptr;
	struct curl_slist* headers = nullptr;
	static const char buf[] = "Expect:";
	if (!strFile.empty()) {
		curl_formadd(&post, &last,
			CURLFORM_COPYNAME, "file",
			CURLFORM_FILE, strFile.c_str(),
			CURLFORM_END);
	}

	//配置参数
	for (auto iter = mapFields.begin(); iter != mapFields.end(); iter++)
	{
		curl_formadd(&post, &last,
			CURLFORM_COPYNAME, iter->first.c_str(),
			CURLFORM_COPYCONTENTS, iter->second.c_str(),
			CURLFORM_END);
	}
	headers = curl_slist_append(headers, buf);
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);  //表单
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, true);//是否需要进度
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);//连接时间
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	curl_formfree(post);
	curl_slist_free_all(headers);
	return res;
}

int32_t CHttpClient::Get(const std::string& strUrl, std::string& strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (nullptr == curl)
		return CURLE_FAILED_INIT;

	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}

	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&strResponse);
	/*
	* 当多个线程都是用超时处理的时候, 同时主线程中有sleep或是wait等操作
	* 如果不设置这个选项,libcurl将会发信号打断这个wait从而导致程序退出
	*/
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int32_t CHttpClient::Posts(const std::string& strUrl, const std::string& strPost
	, std::string& strResponse, const std::string& strCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (nullptr == curl)
		return CURLE_FAILED_INIT;

	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (strCaPath.empty())
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		//缺省情况就是PEM,所以无需设置,另外支持DER
		//curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, strCaPath.c_str());
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int32_t CHttpClient::Gets(const std::string& strUrl, std::string& strResponse
	, const std::string& strCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (nullptr == curl)
		return CURLE_FAILED_INIT;

	if (m_bDebug)
	{
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);
		curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, OnDebug);
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION ,NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (strCaPath.empty())
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, strCaPath.c_str());
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}



