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
	//节点路径 读取单例 zkconfig
	using config_map_type_t = std::map<std::string, std::pair<CUtilLoadBase*, ZkOperation*>>;

	//初始化zk连接
	bool InitZookeeper(std::string& strHost, std::string& strNormalPath);
	void InitAddWatchFile();
	//销毁释放zk连接
	void DestoryZookeeper();
	//创建服务节点
	int CreateServerNode();
	//监控列表
	int WatchAndGetMapConfigList();
	//监控数据
	int WatchAndGetConfigContent();
	void SetErrorInfo(const std::string& strErrorInfo) const;

	bool WaitAllConfigReady(int iTimeout);

public:
	void AddConfigFile(const std::string& strFileName, CUtilLoadBase* pLoadBase, ZkOperation* pZookeeperConfig);
	void DeleteConfigFile(const std::string& strFileName);
	bool UpdateConfig(const std::string& strFileName, const std::string& strContent);
	bool IsLoadComplete();
private:
	//zoo handle状态变更回调
	static void ZooHandleEventWatcher(
		zhandle_t*,						//handle句柄 
		int iType, 						//事件类型
		int iState, 					//连接状态
		const char* szPath, 			//触发监视事件的znode节点路径(NULL为ZOO_SESSION_EVENT)
		void* pContext);				//自定义传入回调数据

	static void CreateServerNodeComplete(
		int iRet,
		const char* pName,
		const void* pContext);

	//地图配置文件列表状态的回调
	static void MapConfigListEventWatcher(
		zhandle_t* pZooHandle,
		int iType, int iState,
		const char* szPath,
		void* pContext);
	//地图配置文件列表变更内容的回调
	static void MapConfigListEventComplete(
		int iRet,
		const struct
		String_vector* pStrings,
		const void* pContext);
	//配置文件内容状态的回调
	static void ConfigContentEventWatcher(
		zhandle_t* pZooHandle,
		int iType,
		int iState,
		const char* szPath,
		void* pContext);
	//配置文件内容变更的回调
	static void ConfigContentEventComplete(
		int iRet,
		const char* iValue,
		int iLength,
		const struct Stat* stStat,
		const void* pContext);
	//设置错误信息完成的回调
	static void SetDataSilentComplete(int iRet, const struct Stat* stStat, const void* szContext) {}
public:
	std::string								m_strHost = "127.0.0.1:2181";
	std::string 							m_strNormalConfigPath = "/normal_config";
	zhandle_t*								m_pZooHandle = NULL;
	config_map_type_t						m_stConfigLoadMap;			//需要load配置集合
	size_t                  				m_uiHasCompletedNum = 0;	//已经load配置数量

};
