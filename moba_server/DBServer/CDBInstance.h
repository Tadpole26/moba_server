#pragma once
#include "mongo_db.h"
#include "mongo_mgr.h"
#include "singleton.h"
#include "CDbInstance.h"

class CDBInstance
{
public:
	CDBInstance();
	~CDBInstance();

	bool Init();
	void UnInit();
	void OnTimer();

	bool QueryData(sDBSelectReq& stRequest);
	bool QuerySelectData(sDBSelectReq& stRequest, fn_RetCall fnCall, size_t& uiRetNum);
	bool QueryMultiData(sDBSelectReq& stRequest, fn_RetCall fnCall, size_t& uiRetNum
		, int64 llBeginId, int32 iNum);
	int64 QueryCount(sDBSelectReq& stRequest);

	void PushGame(sDBRequest& stRequest);

public:
	MongoDb m_oQuery;										//同步处理玩家链接mongo
	MongoMgr m_oQueryThread[DB_THREAD_NUM];					//异步处理玩家链接mongo
	MongoDb m_oMailQuery;									//同步获取全局邮件
};

#define gDBInstance Singleton<CDBInstance>::getInstance()
