#pragma once

#include <string>
#include "db_struct.h"
#include "dtype.h"
#include "mongo_base.h"
#include <exception>
#include <map>

class MongoDb : public MongoBase
{
public:
	MongoDb();
	~MongoDb();
public:
	bool DBUpdate(const std::string& collection, stream::document& query, stream::document& obj);
	bool DBExec(sDBRequest& request);
	bool DBSelectOne(sDBSelectReq& request, std::string* pRetJson = nullptr);
	//返回一级多个
	bool DBSelect(sDBSelectReq& request, fn_RetCall& fnCall, size_t& retNum);
	bool DBSelect(sDBSelectReq& request, fn_RetJsonCall& fnCall, size_t& retNum);
	bool DBSelectMore(sDBSelectReq& request, fn_RetCall& fnCall, size_t& retNum, int64 qwBeginId, int32 limit);
	bool DBSelectMoreJson(sDBSelectReq& request, fn_RetJsonCall& fnCall, size_t& retNum, int64 qwBeginId, int32 limit);
	bool DBInsert(const std::string& ns, const bsoncxx::document::view& query);
	bool IsExsitLst(sDBRequest& request);
	//一级列表计数
	int64 DBCount(sDBSelectReq& request);
	//只针对数字型得字段增加数值
	bool DBIncValue(sDBRequest& request, sDBRet* pRet = nullptr);
	//删除整个集合、慎用!!!!!!!
	bool DropCollection(const std::string& strCol);
	//修改增加,只针对数字型得字段增加数值
	bool DBIncUpdate(sDBRequest& request);
protected:
	//慎用，用于更新所有人得数据状态
	bool DBUpdateMany(sDBRequest& request);
	//只适用于关系型一级结构数据、字段为基础类型
	//不支持字段为复合结构、复合打的操作自己特殊处理
	bool DBInsertLog(sDBRequest& request);
	bool DBDelete(sDBRequest& request);
	bool DBUpdate(sDBRequest& request);
	bool DBInsertUpdate(sDBRequest& request);
	//第二层为数据Object
	bool DBUpdateObject(sDBRequest& request);
	//删除单个行得列
	bool DBDropParam(sDBReqBase& request);

	//适用于二级列表型数据，字段为基础类型
	//不支持字段为复合结构、复合得操作自己特殊处理
	bool DBInsertLogLst(sDBRequest& request);
	bool DBInsertLst(sDBRequest& request, bool* pMatch = nullptr);
	bool DBDeleteLst(sDBRequest& request);
	bool DBUpdateLst(sDBRequest& request, bool* pMatch = nullptr);
	bool DBInsertUpdateLst(sDBRequest& request, bool bUpFirst = true);

	virtual void DoError(const std::string& funcName, int lineno);

private:
	bool SelectList(sDBRet& ret, fn_RetCall& fnCall, const std::string& ns,
		stream::document& query, size_t& retNum, int iToReturn, int iToSkip = 0,
		stream::document* fieldsToReturn = nullptr, stream::document* pSortdoc = nullptr);
	bool SelectList(fn_RetJsonCall& fnCall, const std::string& ns,
		stream::document& query, size_t& retNum, int iToReturn, int iToSkip = 0,
		stream::document* fieldsToReturn = nullptr, stream::document* pSortdoc = nullptr);
	bool SelectOne(sDBRet& ret, const bsoncxx::document::view& values);
	void ReadDBValue(const bsoncxx::document::element& objv, sDBMap& mapRet);

	void MakeUpdate(sDBRequest& request, stream::document& jbuild);
	bool MakeCon(sDBReqBase& request, stream::document& qer);
	bool MakeConMany(sDBReqBase& request, stream::document& qer);
	bool MakeConDelete(sDBReqBase& request, stream::document& qer, bool& bMul);
	bool MakeConDeleteLst(sDBReqBase& request, stream::document& qer, stream::document& jDel);
	void MakeUpdateObject(sDBRequest& request, stream::document& jbuild);
	void MakeUpdateLst(sDBRequest& request, stream::document& jbuild);
	void MakeInsertLst(sDBRequest& request, stream::document& jbuild);
	void MakeInsertLogLst(sDBRequest& request, stream::document& jbuild);

	void MakeBsonFields(sDBReqBase& request, stream::document& jbuild);
	void MakeBsonMap(sDBMap& con, stream::document& jbuild, const std::string& strIndex = "");

	//用于一级、二级KEY得生成
	size_t MakeKeyBson(sDBMap& con, const std::string& key, const std::string& keyEx
		, stream::document& jbuild, bool bErase = false);
};
