#pragma once
#include "http_param.h"
#include "msg_queue.h"
#include "util_time.h"
#include <list>


class CHttpRequest
{
protected:
	//账号请求操作类型
	enum eAccountHttpAction
	{
		eNotifyOnline = 1,			//账号上线通知
		eNotifyLogout = 2,			//账号退出

		eLoginUser,					//登录
		eNotifyWallow,				//查询当前防沉迷状态
		eGetUniqRolId,				//提起全局唯一角色名称和ID
		eModifyName,				//修改角色名字
		eNotifyClearOnlineNum,		//清空在线人数记录
		eNotifyOnlineNum,			//统计在线人数
		eUpLoadLogFile,				//上传战斗文件
		eWChatMsg,					//微信消息
		eRoleUpGrade,				//角色升级
		eServerExit,				//进程退出

		eAccHttpEnd,
	};

	enum eSignMakeSort
	{
		eSignMakeAsc,				//升序
		eSignMakeDesc				//降序
	};

	std::string szHttpUrl[eAccHttpEnd];

public:
	CHttpRequest();
	virtual ~CHttpRequest();

public:
	bool Init(const std::string& szHost);
	//用户上线通知
	void NotifyOnline(int64 llUserId, uint32 uiServerId, uint32 uiAreaNo);
	//用户上线通知
	void NotifyLogin(int64 llUserId, uint32 uiServerId, uint32 uiAreaNo, const std::string& strPwd);
	//用户退出通知
	void NotifyLogout(int64 llUserId, uint32 uiServerId, uint32 uiAreaNo);
	//查询当前防沉迷状态
	void NotifyWallow(int64 llUserId);
	//唯一角色名称和ID
	void GetUniqRolId(int64 llUserId, const std::string&, uint32 uiServerId, uint32 uiAreaNo);
	void ModifyRoleName(int64 llUserId, const std::string&, uint32 uiServerId, uint32 uiAreaNo);
	//清空在线人数记录
	void NotifyClearOnlineNum(uint32 uiServerId, uint32 uiAreaNo);
	//统计在线人数
	void NotifyOnlineNum(uint64 uiNum, uint32 uiServerId, uint32 uiWaitNum
		, uint32 uiAreaNo, const std::string& strIp
		, uint32 uiPort, uint32 uiMaxNum, const std::string& strVersion);
	//短信发送
	void SendChatMsg(const std::string& strMsg);
	//进程关闭成功返回
	void ServerExitRet(uint32 uiArea, int iServerType);

	virtual void OnTimer();
protected:
	std::string MakeSign(const std::string& strPrivateKey, eSignMakeSort sortType, CHttpParam& param);
	virtual void Run(CHttpParam& param) = 0;

	std::string _strHost;
	NumTimer<30> _tmrMsg;
	std::list<std::string> _lstMsg;
private:
	void SendWChatMsg(const std::string& strMsg);
};
