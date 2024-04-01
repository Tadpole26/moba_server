#pragma once

#ifdef _WIN32
#include <curl.h>
#else 
#include <curl/curl.h>
#endif //WIN32

#include <string>
#include <map>
using namespace std;

class CHttpClient
{
public:
	CHttpClient();
	~CHttpClient();

public:
	/*
	*  HTTP POST请求
	* strUrl 输入参数，请求Url地址
	* strPost 输入参数, 使用如下格式
	* strResponse 输出参数,返回得内容
	* return 返回是否Post成功
	*/
	int32_t Post(const std::string& strUrl, const std::string& strPost, std::string& strResponse);
	int32_t Post(const std::string& strUrl, std::map<std::string, std::string>& mapFields
		, const std::string& strFile, std::string& strResponse);
	/*
	* HTTP GET请求
	* strUrl 输入参数,请求得Url地址
	* strResponse 输出参数,返回的内容
	* return 返回是否Post成功
	*/
	int32_t Get(const std::string& strUrl, std::string& strResponse);
	/*
	* HTTPS POST请求，无证书版本
	* strUrl 输入参数，请求的Url地址
	* strPost 输入参数，使用如下格式
	* strResponse 输出参数，返回的内容
	* strCaPath 输入参数，为CA证书的路径，如果输入为NULL,则不验证服务器端证书的有效性
	* return 返回是否Post成功
	*/
	int32_t Posts(const std::string& strUrl, const std::string& strPost
		, std::string& strResponse, const std::string& strCaPath);
	/*
	* HTTPS GET请求，无证书版本
	* strUrl 输入参数，请求的Url地址
	* strResponse 输出参数，返回的内容
	* strCaPath 输入参数，为CA证书的路径，如果输入为NULL，则不验证服务器端证书的有效性
	* return 返回是否Post成功
	*/
	int32_t Gets(const std::string& strUrl, std::string& strResponse
		, const std::string& strCaPath);

public:
	void SetDebug(bool bDebug) { m_bDebug = bDebug; }
	static size_t OnWriteData(void* pBuffer, size_t uiSize
		, size_t uiMemB, void* pVoid);
	static int32_t OnDebug(CURL* pURL, curl_infotype iType
		, char* pData, size_t uiSize, void* pVoid);

public:
	bool m_bDebug = false;
};