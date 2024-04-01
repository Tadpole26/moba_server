#pragma once
#include "CUser.h"
#include <list>
#include "objects_pool.h"


using PLAYER_MAP = std::map<int64, CUser*>;

class CUserManager
{
public:
	CUserManager();
	~CUserManager();

	bool Init();
	bool GetLoadFinish() const { return m_bLoadFinish; }
	void SetLoadFinish(bool val) { m_bLoadFinish = val; }
public:
	//�ȴ��������
	CUser* AddNewWaitCheckPlayer(int64 llUid);
	void AddWaitCheckPlayer(CUser* pPlayer);
	void DelWiatCheckPlayer(int64 llUid);
	CUser* GetWaitCheckPlayer(int64 llUid);
	size_t GetWaitCheckPlayerSize() { return m_lstWaitCheckPwd.size(); }

	//������֤����Ĳ��������
	CUser* GetCheckingPwdPlayer(int64 llUid);
	void DelCheckingPwdPlayer(int64 llUid);

	//������Ϸ�е����
	CUser* GetCheckingPlayer(int64 llUid);
	void AddCheckingUser(int64_t llUid);

	//������Ϸ�е����
	CUser* AddInGamePlayer(int64 llUid);
	void DelGamePlayer(int64 llUid);
	CUser* GetInGamePlayer(int64 llUid);
	size_t GetInGamePlayerSize() { return m_mapInGame.size(); }

	//��ʱ����
	void OnTimer(uint32 uiDwTm);
	void OnTimerCheckPwd();				//�����û���¼�б�
	void OnTimerCheckingPwd();			//�����û���¼�б�
	void OnTimerChecking();				//�����û���¼�б�
	void OnTimerRandNotice();			//������Ϣ


	void PushThreadConn(CUser* pPlayer);


	CUser* GetPlayer(int64 llPlayerId);

	void SendToAllPlayer(const std::string& strMsg, msg_id_t usModule, msg_id_t usCmd, std::map<uint16, std::vector<uint16>>& stThreadToConnMap);
	void SendToAllPlayer(const tagMsgHead* pMyNetMsgHead);
	//�������,ֻ�����һ���µ�
	void SendNoticeAllPlayer(const std::string& stMsg, msg_id_t usModule, msg_id_t usCmd, uint32 uiInterval);
	bool SendToPlayer(int64 llPlayerId, const tagMsgHead* pNetMsgHead);
	void ResetSecLogin();
	void AddSecLogin();
	void OnTimerLineUp();
	bool IsFull();
	bool ReLoginUser(CUser* pPlayer);

private:
	struct rand_notice_t
	{
		std::string m_strMsg;
		msg_id_t	m_usModule = 0;
		msg_id_t	m_usCmd = 0;
		uint32		m_uiInterval = 0;
		time_t		m_llLast = GetCurrTime();
	};

	std::map<int64, rand_notice_t> m_mapRandNotice;

	PLAYER_MAP m_mapInGame;											//������Ϸ�е����
	PLAYER_MAP m_mapChecking;										//���ڴ���Ϸ���������ݵ����
	PLAYER_MAP m_mapCheckingPwd;									//���ڼ����¼��Ч��
	std::list<CUser*> m_lstWaitCheckPwd;							//�Ŷӵȴ���֤��¼��Ч��
	std::map<uint16, std::vector<uint16>> m_mapThreadToConn;		//�߳�id��Ӧ���������id����

	ObjectsPool<CUser>* m_pUserPool = nullptr;						//����ڴ��

	NumTimer<60000> m_stOnLineUp;
	NumTimer<10> m_stLineUp;
	NumTimer<1000> m_stRandNotice;

	uint32 m_uiSecLogin = 0;										//��ǰ���Ѿ���¼������
	uint32 m_uiPreSecLogin = 0;										//��һ���Ѿ���¼������
	time_t m_llPreLogin = GetCurrTime();	

	bool m_bLoadFinish = false;

};