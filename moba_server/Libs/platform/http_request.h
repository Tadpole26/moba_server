#pragma once
#include "http_param.h"
#include "msg_queue.h"
#include "util_time.h"
#include <list>


class CHttpRequest
{
protected:
	//�˺������������
	enum eAccountHttpAction
	{
		eNotifyOnline = 1,			//�˺�����֪ͨ
		eNotifyLogout = 2,			//�˺��˳�

		eLoginUser,					//��¼
		eNotifyWallow,				//��ѯ��ǰ������״̬
		eGetUniqRolId,				//����ȫ��Ψһ��ɫ���ƺ�ID
		eModifyName,				//�޸Ľ�ɫ����
		eNotifyClearOnlineNum,		//�������������¼
		eNotifyOnlineNum,			//ͳ����������
		eUpLoadLogFile,				//�ϴ�ս���ļ�
		eWChatMsg,					//΢����Ϣ
		eRoleUpGrade,				//��ɫ����
		eServerExit,				//�����˳�

		eAccHttpEnd,
	};

	enum eSignMakeSort
	{
		eSignMakeAsc,				//����
		eSignMakeDesc				//����
	};

	std::string szHttpUrl[eAccHttpEnd];

public:
	CHttpRequest();
	virtual ~CHttpRequest();

public:
	bool Init(const std::string& szHost);
	//�û�����֪ͨ
	void NotifyOnline(int64 llUserId, uint32 uiServerId, uint32 uiAreaNo);
	//�û�����֪ͨ
	void NotifyLogin(int64 llUserId, uint32 uiServerId, uint32 uiAreaNo, const std::string& strPwd);
	//�û��˳�֪ͨ
	void NotifyLogout(int64 llUserId, uint32 uiServerId, uint32 uiAreaNo);
	//��ѯ��ǰ������״̬
	void NotifyWallow(int64 llUserId);
	//Ψһ��ɫ���ƺ�ID
	void GetUniqRolId(int64 llUserId, const std::string&, uint32 uiServerId, uint32 uiAreaNo);
	void ModifyRoleName(int64 llUserId, const std::string&, uint32 uiServerId, uint32 uiAreaNo);
	//�������������¼
	void NotifyClearOnlineNum(uint32 uiServerId, uint32 uiAreaNo);
	//ͳ����������
	void NotifyOnlineNum(uint64 uiNum, uint32 uiServerId, uint32 uiWaitNum
		, uint32 uiAreaNo, const std::string& strIp
		, uint32 uiPort, uint32 uiMaxNum, const std::string& strVersion);
	//���ŷ���
	void SendChatMsg(const std::string& strMsg);
	//���̹رճɹ�����
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
