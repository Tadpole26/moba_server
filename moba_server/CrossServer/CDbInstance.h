#pragma once
#include "mongo_db.h"
#include "mongo_mgr.h"
#include "singleton.h"
#include "CDbInstance.h"

class CDbInstance
{
public:
	CDbInstance();
	~CDbInstance();

	bool Init();
	void UnInit();
	void OnTimer();

	bool QueryData(sDBSelectReq& stRequest);
	bool QueryMultiData(sDBSelectReq& stRequest, fn_RetCall fnCall, size_t& uiRetNum
		, int64 llBeginId, int32 iNum);
	int64 QueryCount(sDBSelectReq& stRequest);

	void PushWrite(sDBRequest& stRequest);
	bool Exec(sDBRequest& stRequest);

private:
	MongoDb m_oQuery;
	MongoMgr m_oQueryThread;
};

#define CROSS_DB_INS Singleton<CDbInstance>::getInstance()
