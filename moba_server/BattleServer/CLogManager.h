#pragma once
#include <queue>
#include <mutex>
#include <thread>
#include "singleton.h"
#include "global_define.h"

class CRoom;
class CLogManager
{
public:
	CLogManager();
	~CLogManager();

	//�ύ�������
	void SubmitLoad(bool bThread = true);
	//�ύ������Ϣ
	void SubmitRoomInfo(CRoom* pRoom, bool bThread = true);

	//ս���ձ�����Ϣ�Ƕ��̴߳���,���Ա�������true
	void SendLogMsg(const std::string& strMsg, int iId);
	void SendWorldMsg(const std::string& strMsg, int iId, bool bThread = true);

	void OnTimer(int iTime);

private:
	struct stMsgItem
	{
		SERVER_KIND m_eSvrKind;
		int m_iId;
		std::string m_strMsg;
	};

	std::mutex m_oLockSend;
	std::list<stMsgItem> m_lstMsg;
};

#define gLogManager Singleton<CLogManager>::getInstance()