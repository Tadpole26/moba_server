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
	*  HTTP POST����
	* strUrl �������������Url��ַ
	* strPost �������, ʹ�����¸�ʽ
	* strResponse �������,���ص�����
	* return �����Ƿ�Post�ɹ�
	*/
	int32_t Post(const std::string& strUrl, const std::string& strPost, std::string& strResponse);
	int32_t Post(const std::string& strUrl, std::map<std::string, std::string>& mapFields
		, const std::string& strFile, std::string& strResponse);
	/*
	* HTTP GET����
	* strUrl �������,�����Url��ַ
	* strResponse �������,���ص�����
	* return �����Ƿ�Post�ɹ�
	*/
	int32_t Get(const std::string& strUrl, std::string& strResponse);
	/*
	* HTTPS POST������֤��汾
	* strUrl ��������������Url��ַ
	* strPost ���������ʹ�����¸�ʽ
	* strResponse ������������ص�����
	* strCaPath ���������ΪCA֤���·�����������ΪNULL,����֤��������֤�����Ч��
	* return �����Ƿ�Post�ɹ�
	*/
	int32_t Posts(const std::string& strUrl, const std::string& strPost
		, std::string& strResponse, const std::string& strCaPath);
	/*
	* HTTPS GET������֤��汾
	* strUrl ��������������Url��ַ
	* strResponse ������������ص�����
	* strCaPath ���������ΪCA֤���·�����������ΪNULL������֤��������֤�����Ч��
	* return �����Ƿ�Post�ɹ�
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