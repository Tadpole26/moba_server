#include "pch.h"
#include "http_request.h"
#include "util_random.h"

#define GMWS_PRIVATE_KEY "dbdc6a98e3a8c0ba1e76f0dd32ae47aa"   //gmws私有key

CHttpRequest::CHttpRequest()
{
	szHttpUrl[eNotifyOnline] = "server/account.php";
	szHttpUrl[eNotifyLogout] = "server/account.php";

	szHttpUrl[eLoginUser] = "server/check_session.php";
	szHttpUrl[eNotifyWallow] = "server/account.php";
	szHttpUrl[eGetUniqRolId] = "server/check_role.php";
	szHttpUrl[eModifyName] = "server/update_rolename.php";

	szHttpUrl[eNotifyClearOnlineNum] = "server/clear_online.php";
	szHttpUrl[eNotifyOnlineNum] = "server/online_num.php";
	szHttpUrl[eUpLoadLogFile] = "server/upload.php";
	szHttpUrl[eWChatMsg] = "server/message_sdk.php";
	szHttpUrl[eRoleUpGrade] = "server/role_upgrade.php";
}

CHttpRequest::~CHttpRequest()
{
}

bool CHttpRequest::Init(const std::string& szHost)
{
	_strHost = szHost;
	return true;
}

void CHttpRequest::NotifyLogin(int64 llUserId, uint32 uiServerId, uint32 uiAreaNo, const std::string& strPwd)
{
	time_t llCurTime;
	time(&llCurTime);

	CHttpParam prm;
	prm.m_strHead = szHttpUrl[eLoginUser];
	prm.m_strHost = _strHost;

	//操作类型
	prm.AddParam("act", eLoginUser);
	//账号ID
	prm.AddParam("u", llUserId);
	//当前时间戳
	prm.AddParam("t", llCurTime);
	//serverId
	prm.AddParam("id", uiServerId);
	//大区号
	prm.AddParam("areano", uiAreaNo);
	prm.AddParam("p", strPwd);
	prm.AddParam("k", MakeSign(GMWS_PRIVATE_KEY, eSignMakeAsc, prm));

	prm.m_cType = eLoginUser;
	prm.m_llKey = llUserId;
	prm.m_bRet = true;
	Run(prm);
}

void CHttpRequest::NotifyOnline(int64 llUserId, uint32 uiServerId, uint32 uiAreaNo)
{
	time_t llCurTime;
	time(&llCurTime);

	CHttpParam prm;
	prm.m_strHead = szHttpUrl[eNotifyOnline];
	prm.m_strHost = _strHost;

	//操作类型
	prm.AddParam("act", eNotifyOnline);
	//账号ID
	prm.AddParam("u", llUserId);
	//当前时间戳
	prm.AddParam("t", llCurTime);
	//serverId
	prm.AddParam("id", uiServerId);
	//大区号
	prm.AddParam("areano", uiAreaNo);

	prm.AddParam("k", MakeSign(GMWS_PRIVATE_KEY, eSignMakeAsc, prm));
	prm.m_cType = eNotifyOnline;
	prm.m_llKey = llUserId;
	Run(prm);
}

void CHttpRequest::NotifyLogout(int64 llUserId, uint32 uiServerId, uint32 uiAreaNo)
{
	time_t llCurTime;
	time(&llCurTime);

	CHttpParam prm;
	prm.m_strHead = szHttpUrl[eNotifyLogout];
	prm.m_strHost = _strHost;

	//操作类型
	prm.AddParam("act", eNotifyLogout);
	//账号ID
	prm.AddParam("u", llUserId);
	//当前时间戳
	prm.AddParam("t", llCurTime);
	//serverId
	prm.AddParam("id", uiServerId);
	//大区号
	prm.AddParam("areano", uiAreaNo);

	prm.AddParam("k", MakeSign(GMWS_PRIVATE_KEY, eSignMakeAsc, prm));
	prm.m_cType = eNotifyLogout;
	prm.m_llKey = llUserId;
	Run(prm);
}

void CHttpRequest::NotifyWallow(int64 llUserId)
{
	time_t llCurTime;
	time(&llCurTime);

	CHttpParam prm;
	prm.m_strHead = szHttpUrl[eNotifyWallow];
	prm.m_strHost = _strHost;

	//操作类型
	prm.AddParam("act", eNotifyWallow);
	//账号ID
	prm.AddParam("u", llUserId);
	//当前时间戳
	prm.AddParam("t", llCurTime);

	prm.AddParam("k", MakeSign(GMWS_PRIVATE_KEY, eSignMakeAsc, prm));
	prm.m_cType = eNotifyWallow;
	prm.m_llKey = llUserId;
	Run(prm);
}

void CHttpRequest::GetUniqRolId(int64 llUserId, const std::string& strRolName, uint32 uiServerId, uint32 uiAreaNo)
{
	time_t llCurTime;
	time(&llCurTime);

	CHttpParam prm;
	prm.m_strHead = szHttpUrl[eGetUniqRolId];
	prm.m_strHost = _strHost;

	//操作类型
	prm.AddParam("act", eGetUniqRolId);
	//账号ID
	prm.AddParam("u", llUserId);
	//当前时间戳
	prm.AddParam("t", llCurTime);
	//serverId
	prm.AddParam("id", uiServerId);
	//大区号
	prm.AddParam("areano", uiAreaNo);

	prm.AddParam("k", MakeSign(GMWS_PRIVATE_KEY, eSignMakeAsc, prm));

	prm.AddParam("n", strRolName);
	prm.m_cType = eGetUniqRolId;
	prm.m_llKey = llUserId;
	prm.m_bRet = true;
	Run(prm);
}

void CHttpRequest::ModifyRoleName(int64 llUserId, const std::string& strRolName, uint32 uiServerId, uint32 uiAreaNo)
{
	time_t llCurTime;
	time(&llCurTime);

	CHttpParam prm;
	prm.m_strHead = szHttpUrl[eModifyName];
	prm.m_strHost = _strHost;

	//操作类型
	prm.AddParam("act", eModifyName);
	//账号ID
	prm.AddParam("u", llUserId);
	//当前时间戳
	prm.AddParam("t", llCurTime);
	//serverId
	prm.AddParam("id", uiServerId);
	//大区号
	prm.AddParam("areano", uiAreaNo);

	prm.AddParam("k", MakeSign(GMWS_PRIVATE_KEY, eSignMakeAsc, prm));

	prm.AddParam("n", strRolName);
	prm.m_cType = eModifyName;
	prm.m_llKey = llUserId;
	prm.m_bRet = true;
	Run(prm);
}

void CHttpRequest::NotifyClearOnlineNum(uint32 uiServerId, uint32 uiAreaNo)
{
	time_t llCurTime;
	time(&llCurTime);

	CHttpParam prm;
	prm.m_strHead = szHttpUrl[eNotifyClearOnlineNum];
	prm.m_strHost = _strHost;

	//操作类型
	prm.AddParam("act", eModifyName);
	//当前时间戳
	prm.AddParam("t", llCurTime);
	//serverId
	prm.AddParam("id", uiServerId);
	//大区号
	prm.AddParam("areano", uiAreaNo);

	prm.AddParam("sign", MakeSign(GMWS_PRIVATE_KEY, eSignMakeAsc, prm));

	prm.m_cType = eNotifyClearOnlineNum;
	prm.m_llKey = uiServerId;
	prm.m_bRet = true;
	Run(prm);
}

void CHttpRequest::NotifyOnlineNum(uint64 uiNum, uint32 uiServerId, uint32 uiWaitNum
	, uint32 uiAreaNo, const std::string& strIp
	, uint32 uiPort, uint32 uiMaxNum, const std::string& strVersion)
{
	time_t llCurTime;
	time(&llCurTime);

	CHttpParam prm;
	prm.m_strHead = szHttpUrl[eNotifyOnlineNum];
	prm.m_strHost = _strHost;

	//操作类型
	prm.AddParam("act", eNotifyOnlineNum);
	//当前时间戳
	prm.AddParam("t", llCurTime);
	//serverId
	prm.AddParam("id", uiServerId);
	prm.AddParam("num", uiNum);
	prm.AddParam("waitnum", uiWaitNum);
	prm.AddParam("maxnum", uiMaxNum);
	prm.AddParam("areano", uiAreaNo);
	prm.AddParam("port", uiPort);
	prm.AddParam("ip", strIp);
	prm.AddParam("ver", strVersion);

	prm.AddParam("sign", MakeSign(GMWS_PRIVATE_KEY, eSignMakeAsc, prm));

	prm.m_cType = eNotifyOnlineNum;
	prm.m_llKey = uiServerId;
	prm.m_bRet = true;
	Run(prm);
}

void CHttpRequest::SendChatMsg(const std::string& strMsg)
{
	std::string strDest = strMsg;
	if (_lstMsg.size() == 0)
		_tmrMsg.ResetOn();

	_lstMsg.push_back(strDest);
	//if (_lstMsg.size() >= 10)
	//	SendAllMsg();
}

void CHttpRequest::ServerExitRet(uint32 uiArea, int iServerType)
{
	CHttpParam prm;
	prm.m_strHead = szHttpUrl[eServerExit];
	prm.m_strHost = _strHost;

	//操作类型
	prm.AddParam("act", eServerExit);
	//当前时间戳
	prm.AddParam("t", GetCurrTime());
	//serverId
	prm.AddParam("server", iServerType);

	prm.AddParam("sign", MakeSign(GMWS_PRIVATE_KEY, eSignMakeAsc, prm));

	prm.m_cType = eServerExit;
	prm.m_llKey = 0;
	prm.m_bRet = false;
	prm.m_bPost = true;
	Run(prm);
}

void CHttpRequest::SendWChatMsg(const std::string& strMsg)
{
	time_t llCurTime;
	time(&llCurTime);
	CHttpParam prm;
	prm.m_strHead = szHttpUrl[eWChatMsg];
	prm.m_strHost = _strHost;

	prm.AddParam("content", strMsg);
	prm.AddParam("t", llCurTime);
	prm.AddParam("sign", MakeSign(GMWS_PRIVATE_KEY, eSignMakeAsc, prm));
	prm.m_cType = eWChatMsg;
	prm.m_llKey = 0;
	prm.m_bRet = false;
	prm.m_bPost = true;
	Run(prm);
}

void CHttpRequest::OnTimer()
{
	if (_lstMsg.size() == 0)
		return;

	//if (_tmrMsg.On())
	//	SendAllMsg();
}

std::string CHttpRequest::MakeSign(const std::string& strPrivateKey, eSignMakeSort sortType, CHttpParam& param)
{
	std::string strMakeMd5("");
	bool bFirst = true;
	std::map<std::string, std::string>& mapParam = param.GetMapFields();
	for (auto iter = mapParam.begin(); iter != mapParam.end(); ++iter)
	{
		if (bFirst)
			bFirst = false;
		else
			strMakeMd5 += ":";
		strMakeMd5 += iter->first + "=" + iter->second;
	}
	strMakeMd5 += ":" + strPrivateKey;
	return make_md5_32(strMakeMd5);
}
