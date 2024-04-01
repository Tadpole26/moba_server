#pragma once

#include <iostream>
#include <functional>
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "CConstConfig.h"
#include "util_load_base.h"
extern "C"
{
	#include "zookeeper.h"
}

//#include "CGameBaseConfig.h"


class ZkOperation
{
public:
	static const char* Type2String(int iEventType);
	static const char* State2String(int iState);
	static const char* Errno2String(int iRet);
public:
	//typedef std::function<bool(const ZkOperation&)> reload_func_type;
	//�ڵ�·�� ��ȡ���� zkconfig
	using config_map_type_t = std::map<std::string, std::pair<CUtilLoadBase*, ZkOperation*>>;

	//��ʼ��zk����
	bool InitZookeeper(std::string& strHost, std::string& strNormalPath);
	void InitAddWatchFile();
	//�����ͷ�zk����
	void DestoryZookeeper();
	//��������ڵ�
	int CreateServerNode();
	//����б�
	int WatchAndGetMapConfigList();
	//�������
	int WatchAndGetConfigContent();
	void SetErrorInfo(const std::string& strErrorInfo) const;

	bool WaitAllConfigReady(int iTimeout);

public:
	void AddConfigFile(const std::string& strFileName, CUtilLoadBase* pLoadBase, ZkOperation* pZookeeperConfig);
	void DeleteConfigFile(const std::string& strFileName);
	bool UpdateConfig(const std::string& strFileName, const std::string& strContent);
	bool IsLoadComplete();
private:
	//zoo handle״̬����ص�
	static void ZooHandleEventWatcher(
		zhandle_t*,						//handle��� 
		int iType, 						//�¼�����
		int iState, 					//����״̬
		const char* szPath, 			//���������¼���znode�ڵ�·��(NULLΪZOO_SESSION_EVENT)
		void* pContext);				//�Զ��崫��ص�����

	static void CreateServerNodeComplete(
		int iRet,
		const char* pName,
		const void* pContext);

	//��ͼ�����ļ��б�״̬�Ļص�
	static void MapConfigListEventWatcher(
		zhandle_t* pZooHandle,
		int iType, int iState,
		const char* szPath,
		void* pContext);
	//��ͼ�����ļ��б������ݵĻص�
	static void MapConfigListEventComplete(
		int iRet,
		const struct
		String_vector* pStrings,
		const void* pContext);
	//�����ļ�����״̬�Ļص�
	static void ConfigContentEventWatcher(
		zhandle_t* pZooHandle,
		int iType,
		int iState,
		const char* szPath,
		void* pContext);
	//�����ļ����ݱ���Ļص�
	static void ConfigContentEventComplete(
		int iRet,
		const char* iValue,
		int iLength,
		const struct Stat* stStat,
		const void* pContext);
	//���ô�����Ϣ��ɵĻص�
	static void SetDataSilentComplete(int iRet, const struct Stat* stStat, const void* szContext) {}
public:
	std::string								m_strHost = "127.0.0.1:2181";
	std::string 							m_strNormalConfigPath = "/normal_config";
	zhandle_t*								m_pZooHandle = NULL;
	config_map_type_t						m_stConfigLoadMap;			//��Ҫload���ü���
	size_t                  				m_uiHasCompletedNum = 0;	//�Ѿ�load��������

};
