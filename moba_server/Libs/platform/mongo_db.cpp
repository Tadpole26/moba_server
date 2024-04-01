#include "pch.h"
#include "mongo_db.h"
#include "mongocxx/exception/query_exception.hpp"
#include "util_file.h"

MongoDb::MongoDb()
{
}

MongoDb::~MongoDb()
{
}

bool MongoDb::DBExec(sDBRequest& request)
{
	if (request.type() != eDB_UpdateMany && request.type() != eDB_DropTable && !request.Check())
	{
		Log_Error("key error! %s:%s type:%d, class %s", request.tbl().c_str()
			, request.sectbl().name().c_str(), request.type(), typeid(*this).name());
		return false;
	}

	bool bRet = true;
	tagSecTblDesc::eDBSecType eSecType = request.sectbl().type();
	switch (request.type())
	{
	case eDB_Insert:
		if (eSecType == tagSecTblDesc::eST_List)
			bRet = DBInsertUpdateLst(request, false);
		else if (eSecType == tagSecTblDesc::eST_None)
			bRet = DBUpdate(request);
		else if (eSecType == tagSecTblDesc::eST_Object)
			bRet = DBUpdateObject(request);
		break;
	case eDB_Update:
		if (eSecType == tagSecTblDesc::eST_List)
			bRet = DBInsertUpdateLst(request);
		else if (eSecType == tagSecTblDesc::eST_None)
			bRet = DBUpdate(request);
		else if (eSecType == tagSecTblDesc::eST_Object)
			bRet = DBUpdateObject(request);
		break;
	case eDB_InsertUpdate:
		if (eSecType == tagSecTblDesc::eST_List)
			bRet = DBInsertUpdateLst(request);
		else if (eSecType == tagSecTblDesc::eST_None)
			bRet = DBInsertUpdate(request);
		else if (eSecType == tagSecTblDesc::eST_Object)
			bRet = DBUpdateObject(request);
		break;
	case eDB_Delete:
		if (eSecType == tagSecTblDesc::eST_List)
			bRet = DBDeleteLst(request);
		else if (eSecType == tagSecTblDesc::eST_None)
			bRet = DBDelete(request);
		break;
	case eDB_IncValue:
		bRet = DBIncValue(request);
		break;
	case eDB_InsertLog:
		if (eSecType == tagSecTblDesc::eST_None)
			bRet = DBInsertLog(request);
		else
			bRet = DBInsertLogLst(request);
		break;
	case eDB_DropParam:
		bRet = DBDropParam(request);
		break;
	case eDB_UpdateMany:
		bRet = DBUpdateMany(request);
		break;
	case eDB_DropTable:
		bRet = DropCollection(request.tbl());
		break;
	case eDB_IncUpdate:
		bRet = DBIncUpdate(request);
		break;
	default:
		Log_Error("type error! %s:%s type:%d, class:%s", request.tbl().c_str()
			, request.sectbl().name().c_str(), request.type(), typeid(*this).name());
		return false;
	}

	if (!bRet)
	{
		Log_Error("ret error! %s:%s type:%d, class:%s", request.tbl().c_str()
			, request.sectbl().name().c_str(), request.type(), typeid(*this).name());
	}
	return bRet;
}

bool MongoDb::IsExsitLst(sDBRequest& request)
{
	if (!request.Check())
	{
		Log_Error("key error! %s:%s type:%d, class %s", request.tbl().c_str()
			, request.sectbl().name().c_str(), request.type(), typeid(*this).name());
		return false;
	}

	LogName(request.tbl(), request.sectbl().name());
	stream::document jFind, jFields;

	BEGIN_TRY_MD
	{
		std::string strIndex = request.sectbl().name() + "." + request.S_KEY;
		if (!MakeKeyBson(request.Con(), request.S_KEY, strIndex, jFind))
		{
			if (!MakeKeyBson(request.reqMap(), request.S_KEY, strIndex, jFind))
			{
				Log_Error("%s-%s:id is null", request.tbl().c_str(), request.sectbl().name().c_str());
				return false;
			}
		}

		if (!MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, jFind))
		{
			Log_Error("%s-%s:id is null", request.tbl().c_str(), request.sectbl().name().c_str());
			return false;
		}

		std::string fields = request.sectbl().name() + ".$";
		jFields << fields << true;

		return _is_exsit(request.tbl(), jFind, &jFields);
	}
	END_TRY_MD(jFind, jFields);

	return false;
}

bool MongoDb::DBUpdate(sDBRequest& request)
{
	LogName(request.tbl(), request.sectbl().name());
	stream::document jbuild, qer;

	BEGIN_TRY_MD
	{
		if (!MakeCon(request, qer)) return false;
		MakeUpdate(request, jbuild);
		bool bRet = _update_one(request.tbl(), qer, jbuild, true);
		jbuild.clear();
		qer.clear();

		return bRet;
	}
	END_TRY_MD(qer, jbuild);
	return false;
}

bool MongoDb::DBUpdate(const std::string& collection, stream::document& query, stream::document& obj)
{
	if (collection.empty()) return false;

	LogName(collection, "");
	BEGIN_TRY_MD
	{
		return _update_one(collection, query, obj, true);
	}
	END_TRY_MD(query, obj);
	return false;
}

bool MongoDb::DBUpdateMany(sDBRequest& request)
{
	LogName(request.tbl(), request.sectbl().name());
	stream::document jbuild, qer;

	BEGIN_TRY_MD
	{
		MakeConMany(request, qer);
	if (request.sectbl().IsEmpty())
		MakeUpdate(request, jbuild);
	else if (request.sectbl().IsList())
		MakeUpdateLst(request, jbuild);
	else
		MakeUpdateObject(request, jbuild);
	
	return _update_many(request.tbl(), qer, jbuild, false);
	}
	END_TRY_MD(qer, jbuild);
	return false;
}

bool MongoDb::DBSelectOne(sDBSelectReq& request, std::string* pRetJson)
{
	request.clear_sectbl();
	if (!request.Check(sDBRequest::eCKT_KEY))
	{
		Log_Error("key error! %s:%s type:%d, class %s", request.tbl().c_str()
			, request.sectbl().name().c_str(), request.type(), typeid(*this).name());
		return false;
	}
	LogName(request.tbl(), request.sectbl().name());
	stream::document jFields, qer;

	BEGIN_TRY_MD
	{
		if (!MakeCon(request, qer)) return false;
		MakeBsonFields(request, jFields);
		optx op = _find_one(request.tbl(), qer, &jFields);
		if (op == bsoncxx::stdx::nullopt) return true;
		if (pRetJson != nullptr) *pRetJson = bsoncxx::to_json(op.get().view());
		return SelectOne(request.Ret(), op.get().view());
	}
	END_TRY_MD(qer, jFields);
	return false;
}

bool MongoDb::DBSelect(sDBSelectReq& request, fn_RetCall& fnCall, size_t& retNum)
{
	retNum = 0;
	LogName(request.tbl(), request.sectbl().name());
	stream::document jFields, qer;

	BEGIN_TRY_MD
	{
		size_t lstSize = MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer);
		if (!MakeCon(request, qer)) return false;
		MakeBsonFields(request, jFields);

		if (lstSize >= 1)
			return SelectList(request.Ret(), fnCall, request.tbl(), qer, retNum, 0, 0, &jFields);
		else
		{
			Log_Error("must have _id fields!, else use DBSelectMore!!");
			return false;
		}
	}
	END_TRY_MD(qer, jFields);
	return false;
}

bool MongoDb::DBSelect(sDBSelectReq& request, fn_RetJsonCall& fnCall, size_t& retNum)
{
	retNum = 0;
	LogName(request.tbl(), request.sectbl().name());
	stream::document jFields, qer;

	BEGIN_TRY_MD
	{
		size_t lstSize = MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer);
		if (!MakeCon(request, qer)) return false;
		MakeBsonFields(request, jFields);

		if (lstSize >= 1)
			return SelectList(fnCall, request.tbl(), qer, retNum, 0, 0, &jFields);
		else
		{
			Log_Error("must have _id fields!, else use DBSelectMore!!");
			return false;
		}
	}
	END_TRY_MD(qer, jFields);
	return false;
}

bool MongoDb::DBSelectMore(sDBSelectReq& request, fn_RetCall& fnCall,
	size_t& retNum, int64 qwBeginId, int32 limit)
{
	retNum = 0;
	LogName(request.tbl(), request.sectbl().name());
	stream::document jFields, qer;

	BEGIN_TRY_MD
	{
		size_t lstSize = MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer);
		if (!MakeCon(request, qer)) return false;
		MakeBsonFields(request, jFields);

		if (lstSize >= 1)
			return SelectList(request.Ret(), fnCall, request.tbl(), qer, retNum, 0, 0, &jFields);
		else
		{
			stream::document dSort;
			AppendNum(dSort, request.F_KEY, 1, EDCT_EQ);
			AppendNum(qer, request.F_KEY, qwBeginId, EDCT_GT);
			return SelectList(request.Ret(), fnCall, request.tbl(), qer, retNum, limit, 0, &jFields);
		}
	}
	END_TRY_MD(qer, jFields);
	return false;
}

bool MongoDb::DBSelectMoreJson(sDBSelectReq& request, fn_RetJsonCall& fnCall,
	size_t& retNum, int64 qwBeginId, int32 limit)
{
	retNum = 0;
	LogName(request.tbl(), request.sectbl().name());
	stream::document jFields, qer;

	BEGIN_TRY_MD
	{
		size_t lstSize = MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer);
		if (!MakeCon(request, qer)) return false;
		MakeBsonFields(request, jFields);

		if (lstSize >= 1)
			return SelectList(fnCall, request.tbl(), qer, retNum, 0, 0, &jFields);
		else
		{
			stream::document dSort;
			AppendNum(dSort, request.F_KEY, 1, EDCT_EQ);
			AppendNum(qer, request.F_KEY, qwBeginId, EDCT_GT);
			return SelectList(fnCall, request.tbl(), qer, retNum, limit, 0, &jFields);
		}
	}
	END_TRY_MD(qer, jFields);
	return false;
}

bool MongoDb::DBInsert(const std::string& ns, const bsoncxx::document::view& query)
{
	LogName(ns);
	BEGIN_TRY_MD
	{
		return _insert(ns, query);
	}
	END_TRY_MD(ns, bsoncxx::to_json(query));
	return false;
}

int64 MongoDb::DBCount(sDBSelectReq& request)
{
	LogName(request.tbl(), request.sectbl().name());
	stream::document qer;

	BEGIN_TRY_MD
	{
		if (!MakeCon(request, qer)) return false;

		return (int64)_count(request.tbl(), qer);
	}
	END_TRY_MD(qer, qer);
	return 0;
}

bool MongoDb::DBInsertLog(sDBRequest& request)
{
	LogName(request.tbl(), request.sectbl().name());
	stream::document jbuild;

	BEGIN_TRY_MD
	{
		MakeBsonMap(request.reqMap(), jbuild);
		bool bRet = _insert(request.tbl(), jbuild);
		jbuild.clear();
		return bRet;
	}
	END_TRY_MD(jbuild, jbuild);
	return false;
}

bool MongoDb::DBDelete(sDBRequest& request)
{
	LogName(request.tbl(), request.sectbl().name());
	stream::document qer;

	BEGIN_TRY_MD
	{
		bool bMul = false;
	if (!MakeConDelete(request, qer, bMul))
		return false;

	if (bMul)
		return _remove(request.tbl(), qer, false, request.IsDelError());
	else
		return _remove(request.tbl(), qer, true, request.IsDelError());
	}
	END_TRY_MD(qer, qer);
	return false;
}

bool MongoDb::DBInsertUpdate(sDBRequest& request)
{
	LogName(request.tbl(), request.sectbl().name());
	stream::document jbuild, qer;

	BEGIN_TRY_MD
	{
		if (!MakeCon(request, qer)) return false;
		MakeUpdate(request, jbuild);
		return _update_one(request.tbl(), qer, jbuild, true);
	}
	END_TRY_MD(qer, jbuild);
	return false;
}

bool MongoDb::DBUpdateObject(sDBRequest& request)
{
	LogName(request.tbl(), request.sectbl().name());
	stream::document jbuild, qer;

	BEGIN_TRY_MD
	{
		if (!MakeCon(request, qer)) return false;
		MakeUpdateObject(request, jbuild);
		return _update_one(request.tbl(), qer, jbuild, true);
	}
	END_TRY_MD(qer, jbuild);
	return false;
}

bool MongoDb::DBDropParam(sDBReqBase& request)
{
	LogName(request.tbl(), request.sectbl().name());
	stream::document qer, jret;

	BEGIN_TRY_MD
	{
		if (!MakeCon(request, qer)) return false;
		
		stream::document jadd;
		for (const auto& iter : request.Fields())
		{
			jadd << iter << true;
		}
		jret << "$unset" << jadd;
		return _update_one(request.tbl(), qer, jret, true);
	}
	END_TRY_MD(qer, jret);
	return false;
}

bool MongoDb::DBIncValue(sDBRequest& request, sDBRet* pRet)
{
	if (!request.Check())
	{
		Log_Error("key error! %s:%s type:%d, class %s", request.tbl().c_str()
			, request.sectbl().name().c_str(), request.type(), typeid(*this).name());
		return false;
	}

	if (pRet != nullptr)
		pRet->clear();

	LogName(request.tbl(), request.sectbl().name());
	stream::document qer, jret;

	BEGIN_TRY_MD
	{
		if (!MakeCon(request, qer)) return false;
		stream::document jadd;

		switch (request.sectbl().type())
		{
			case tagSecTblDesc::eST_List:
				MakeBson(request.reqMap().MapNum(), jadd, request.sectbl().name() + ".$.");
				break;
			case tagSecTblDesc::eST_Object:
				MakeBson(request.reqMap().MapNum(), jadd, request.sectbl().name() + ".");
				break;
			case tagSecTblDesc::eST_None:
				MakeBson(request.reqMap().MapNum(), jadd);
				break;
			default:
				Log_Error("sectype is error!, %s-%s",
					request.tbl().c_str(), request.sectbl().name().c_str());
				return false;
				break;
		}
		jret << "$inc" << jadd;

		if (pRet != nullptr)
		{
			optx bsQer = _find_and_modify(request.tbl(), qer, jret, true);
			if (bsQer != bsoncxx::stdx::nullopt)
				return SelectOne(*pRet, bsQer.get().view());
		}
		else
			return _update_one(request.tbl(), qer, jret, true);
	}
	END_TRY_MD(qer, jret);
	return false;
}

bool MongoDb::DropCollection(const std::string& strCol)
{
	Log_Custom("dbdrop", "table: %s", strCol.c_str());
	if (strCol.empty()) return true;

	LogName(strCol);
	stream::document qer, jIns;
	BEGIN_TRY_MD
	{
		return _drop_collection(strCol);
	}
	END_TRY_MD(qer, jIns);
	return false;
}

bool MongoDb::DBIncUpdate(sDBRequest& request)
{
	if (!request.Check())
	{
		Log_Error("key error! %s:%s type:%d, class %s", request.tbl().c_str()
			, request.sectbl().name().c_str(), request.type(), typeid(*this).name());
		return false;
	}

	LogName(request.tbl(), request.sectbl().name());
	stream::document qer, jret;

	BEGIN_TRY_MD
	{
		if (!MakeCon(request, qer)) return false;
		stream::document jadd;
		stream::document jset;

		switch (request.sectbl().type())
		{
			case tagSecTblDesc::eST_List:
				MakeBson(request.reqMap().MapNum(), jadd, request.sectbl().name() + ".$.");
				MakeBson(request.reqMap().MapBool(), jset, request.sectbl().name() + ".$.");
				MakeBson(request.reqMap().MapStr(), jset, request.sectbl().name() + ".$.");
				break;
			case tagSecTblDesc::eST_Object:
				MakeBson(request.reqMap().MapNum(), jadd, request.sectbl().name() + ".");
				MakeBson(request.reqMap().MapBool(), jset, request.sectbl().name() + ".");
				MakeBson(request.reqMap().MapStr(), jset, request.sectbl().name() + ".");
				break;
			case tagSecTblDesc::eST_None:
				MakeBson(request.reqMap().MapNum(), jadd);
				MakeBson(request.reqMap().MapBool(), jset);
				MakeBson(request.reqMap().MapStr(), jset);
				break;
			default:
				Log_Error("sectype is error!, %s-%s",
					request.tbl().c_str(), request.sectbl().name().c_str());
				return false;
				break;
		}
		if (!jadd.view().empty())
			jret << "$inc" << jadd;
		if (!jset.view().empty())
			jret << "$set" << jset;

		return _update_one(request.tbl(), qer, jret, true);
	}
	END_TRY_MD(qer, jret);
	return false;
}

bool MongoDb::DBInsertLogLst(sDBRequest& request)
{
	LogName(request.tbl(), request.sectbl().name());
	stream::document qer, jIns;

	BEGIN_TRY_MD
	{
		MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer);
		MakeInsertLogLst(request, jIns);
		return _update_one(request.tbl(), qer, jIns, true);
	}
	END_TRY_MD(qer, jIns);
	return false;
}

bool MongoDb::DBInsertLst(sDBRequest& request, bool* pMatch)
{
	LogName(request.tbl(), request.sectbl().name());
	stream::document qer, jIns;

	BEGIN_TRY_MD
	{
		stream::document jne;
		MakeInsertLst(request, jIns);
		if (!MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer)
			|| !MakeKeyBson(request.reqMap(), request.S_KEY, "$ne", jne))
		{
			Log_Error("id is null! %s-%s"
				, request.tbl().c_str(), request.sectbl().name().c_str());
			return false;
		}
		qer << request.sectbl().name() + "." + request.S_KEY << jne;

		bool bRet = false;
		bRet = _update_one(request.tbl(), qer, jIns, true, pMatch);

		qer.clear();
		jIns.clear();
		return bRet;
	}
	END_TRY_MD(qer, jIns);
	return false;
}

bool MongoDb::DBDeleteLst(sDBRequest& request)
{
	LogName(request.tbl(), request.sectbl().name());
	stream::document qer, jDel;

	BEGIN_TRY_MD
	{
		if (!MakeConDeleteLst(request, qer, jDel))
			return false;

		bool bDelMatch = false;
		bool bRet = _update_one(request.tbl(), qer, jDel, false, &bDelMatch);
		if (!bDelMatch)
		{
			Log_Error("delete row is empty! %s, %s", bsoncxx::to_json(qer).c_str()
				, bsoncxx::to_json(jDel).c_str());
		}
		return bRet;
	}
	END_TRY_MD(qer, jDel);
	return false;
}

bool MongoDb::DBUpdateLst(sDBRequest& request, bool* pMatch)
{
	LogName(request.tbl(), request.sectbl().name());
	stream::document jbuild, qer;

	BEGIN_TRY_MD
	{
		if (!MakeCon(request, qer)) return false;
		MakeUpdateLst(request, jbuild);
		bool bRet = _update_one(request.tbl(), qer, jbuild, false, pMatch);
		jbuild.clear();
		qer.clear();
		return bRet;
	}
	END_TRY_MD(qer, jbuild);
	return false;
}

bool MongoDb::DBInsertUpdateLst(sDBRequest& request, bool bUpFirst)
{
	LogName(request.tbl(), request.sectbl().name());
	bool bMatch = false;

	if (bUpFirst)
	{
		bool bRet = DBUpdateLst(request, &bMatch);
		if (bRet)
		{
			if (!bMatch)
				return DBInsertLst(request);
			return bRet;
		}
		else
		{
			return DBInsertLst(request);
		}
	}
	else
	{
		bool bRet = DBInsertLst(request, &bMatch);
		if (bRet)
		{
			if (!bMatch)
				return DBUpdateLst(request);
			return bRet;
		}
		else
		{
			return DBUpdateLst(request);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
bool MongoDb::SelectList(sDBRet& ret, fn_RetCall& fnCall, const std::string& ns,
	stream::document& query, size_t& retNum, int iToReturn, int iToSkip,
	stream::document* fieldsToReturn, stream::document* pSortdoc)
{
	FORDO_EXEC_BEGIN
	{
		mongocxx::cursor curs = _find_many(ns, query, iToReturn, iToSkip, fieldsToReturn, pSortdoc);
		for (const bsoncxx::document::view& bObj : curs)
		{
			SelectOne(ret, bObj);
			if (!fnCall(ret))
				return false;
			++retNum;
		}
		return true;
	}
	FORDO_EXEC_END(mongocxx::v_noabi::query_exception);
	return false;
}

bool MongoDb::SelectList(fn_RetJsonCall& fnCall, const std::string& ns,
	stream::document& query, size_t& retNum, int iToReturn, int iToSkip,
	stream::document* fieldsToReturn, stream::document* pSortdoc)
{
	FORDO_EXEC_BEGIN
	{
		mongocxx::cursor curs = _find_many(ns, query, iToReturn, iToSkip, fieldsToReturn, pSortdoc);
		for (const bsoncxx::document::view& bObj : curs)
		{
			if (!fnCall(bObj))
				return false;
			++retNum;
		}
		return true;
	}
	FORDO_EXEC_END(mongocxx::v_noabi::query_exception);
	return false;
}

bool MongoDb::SelectOne(sDBRet& ret, const bsoncxx::document::view& values)
{
	ret.clear();
	for (auto& objv : values)
	{
		if (objv.raw() == nullptr) continue;
		std::string strKey = objv.key().to_string();
		if (strKey.empty()) continue;
		ret.AddField(strKey);
		switch (objv.type())
		{
		case bsoncxx::type::k_array:
		{
			bsoncxx::types::b_array vitem = objv.get_array();
			bool bSimple = false;
			if (vitem.value.begin() != vitem.value.end())
			{
				auto atype = vitem.value.begin()->type();
				if (atype != bsoncxx::type::k_document)
					bSimple = true;
			}
			if (bSimple)
			{
				std::list<int64> lstID;
				std::list<std::string> lstStr;
				for (auto& objdata : vitem.value)
				{
					if (objdata.raw() == nullptr) continue;
					if (objdata.type() == bsoncxx::type::k_int32)
						lstID.push_back(objdata.get_int32());
					else if (objdata.type() == bsoncxx::type::k_int64)
						lstID.push_back(objdata.get_int64());
					else if (objdata.type() == bsoncxx::type::k_double)
						lstID.push_back((int64)objdata.get_double());
					else if (objdata.type() == bsoncxx::type::k_utf8)
						lstStr.push_back(objdata.get_utf8().value.to_string());
				}
				if (lstID.size() > 0)
					ret.Ret_Main().AddList(strKey.c_str(), lstID.begin(), lstID.end());
				if (lstStr.size() > 0)
					ret.Ret_Main().AddListStr(strKey.c_str(), lstStr.begin(), lstStr.end());
			}
			else
			{
				sDBSecRet& vecValue = ret.Ret_Vec(strKey.c_str());
				size_t vSize = std::distance(vitem.value.begin(), vitem.value.end());
				vecValue.resize(vSize);
				size_t uiIndex = 0;
				for (auto& objdata : vitem.value)
				{
					if (objdata.type() == bsoncxx::type::k_document)
					{
						bsoncxx::types::b_document vData = objdata.get_document();
						for (auto& item : vData.view())
							ReadDBValue(item, vecValue[uiIndex]);
						++uiIndex;
					}
				}
			}
		}
		break;
		case bsoncxx::type::k_document:
		{
			sDBSecRet& vecValue = ret.Ret_Vec(strKey.c_str());
			sDBMap mapRet;
			bsoncxx::types::b_document doc = objv.get_document();
			for (auto& item : doc.value)
				ReadDBValue(item, mapRet);
			vecValue.push_back(std::move(mapRet));
		}
		break;
		default:
			ReadDBValue(objv, ret.Ret_Main());
			break;
		}
	}
	return true;
}

void MongoDb::ReadDBValue(const bsoncxx::document::element& objv, sDBMap& mapRet)
{
	if (objv.raw() == nullptr) return;
	std::string strKey = objv.key().to_string();
	if (strKey.length() == 0) return;

	switch (objv.type())
	{
	case bsoncxx::type::k_int32:
		mapRet.Add(strKey.c_str(), (objv.get_int32().value));
		break;
	case bsoncxx::type::k_int64:
		mapRet.Add(strKey.c_str(), objv.get_int64().value);
		break;
	case bsoncxx::type::k_date:
		mapRet.AddTime(strKey.c_str(), std::chrono::duration_cast<std::chrono::seconds>(objv.get_date().value));
		break;
	case bsoncxx::type::k_bool:
		mapRet.Add(strKey.c_str(), objv.get_bool().value);
		break;
	case bsoncxx::type::k_double:
		mapRet.AddDouble(strKey.c_str(), objv.get_double().value);
		break;
	case bsoncxx::type::k_utf8:
		mapRet.AddStr(strKey.c_str(), objv.get_utf8().value.to_string());
		break;
	case bsoncxx::type::k_oid:
		mapRet.AddStr(strKey.c_str(), objv.get_oid().value.to_string());
		break;
	case bsoncxx::type::k_binary:
	{
		auto bytes = objv.get_binary();
		std::string ret;
		ret.append((char*)bytes.bytes, bytes.size);
		mapRet.AddBinary(strKey.c_str(), ret);
	}
		break;
	case bsoncxx::type::k_array:
	{
		bsoncxx::types::b_array arr = objv.get_array();
		std::list<int64> lstID;
		std::list<std::string> lstStr;
		for (auto& iter : arr.value)
		{
			if (iter.raw() == nullptr) continue;
			if (iter.type() == bsoncxx::type::k_int32)
				lstID.push_back(iter.get_int32());
			else if (iter.type() == bsoncxx::type::k_int64)
				lstID.push_back(iter.get_int64());
			else if (iter.type() == bsoncxx::type::k_double)
				lstID.push_back((int64)iter.get_double());
			else if (iter.type() == bsoncxx::type::k_utf8)
				lstStr.push_back(iter.get_utf8().value.to_string());
		}
		if (lstID.size() > 0)
			mapRet.AddList(strKey.c_str(), lstID.begin(), lstID.end());
		if (lstStr.size() > 0)
			mapRet.AddListStr(strKey.c_str(), lstStr.begin(), lstStr.end());
	}
		break;
	default:
		break;
	}
}

void MongoDb::MakeUpdate(sDBRequest& request, stream::document& jbuild)
{
	stream::document jadd;
	MakeBsonMap(request.reqMap(), jadd);
	jbuild << "$set" << jadd;
	jadd.clear();
}

bool MongoDb::MakeCon(sDBReqBase& request, stream::document& qer)
{
	bool bExt = false;
	if (request.sectbl().IsList())
	{
		std::string strIndex = request.sectbl().name() + "." + request.S_KEY;
		if (!MakeKeyBson(request.Con(), request.S_KEY, strIndex, qer))
		{
			Log_Error("%s-%s, id is null", request.tbl().c_str()
				, request.sectbl().name().c_str());
			return false;
		}
		bExt = true;
	}

	MakeBson(request.Con().MapNum(), qer, "", bExt ? request.S_KEY : "");
	MakeBson(request.Con().MapStr(), qer, "", bExt ? request.S_KEY : "");
	return true;
}

bool MongoDb::MakeConMany(sDBReqBase& request, stream::document& qer)
{
	MakeBson(request.Con().MapNum(), qer);
	MakeBson(request.Con().MapStr(), qer);
	return true;
}

bool MongoDb::MakeConDelete(sDBReqBase& request, stream::document& qer, bool& bMul)
{
	size_t ret = MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer, true);
	if (ret == 0)
	{
		Log_Error("id is null!, will delete all!, %s-%s"
			, request.tbl().c_str(), request.sectbl().name().c_str());
		return false;
	}
	bMul = (ret > 1);
	if (!MakeCon(request, qer)) return false;
	return true;
}

bool MongoDb::MakeConDeleteLst(sDBReqBase& request, stream::document& qer, stream::document& jDel)
{
	stream::document jadd, jaddSub;

	if (!MakeKeyBson(request.Con(), request.S_KEY, request.S_KEY, jaddSub, true))
	{
		Log_Error("id is null!, will delete all sectable!, %s-%s"
			, request.tbl().c_str(), request.sectbl().name().c_str());
		return false;
	}
	jadd << request.sectbl().name() << jaddSub;
	jDel << "%pull" << jadd;

	if (!MakeKeyBson(request.Con(), request.F_KEY, request.F_KEY, qer, true))
	{
		jadd.clear();
		jaddSub.clear();

		Log_Error("id is null!, will delete all!, %s-%s"
			, request.tbl().c_str(), request.sectbl().name().c_str());
		return false;
	}

	MakeBson(request.Con().MapNum(), qer);
	MakeBson(request.Con().MapStr(), qer);

	jadd.clear();
	jaddSub.clear();
	return true;
}

void MongoDb::MakeUpdateObject(sDBRequest& request, stream::document& jbuild)
{
	stream::document jsub;
	MakeBsonMap(request.reqMap(), jsub, request.sectbl().name() + ".");
	jbuild << "$set" << jsub;
	jsub.clear();
}

void MongoDb::MakeUpdateLst(sDBRequest& request, stream::document& jbuild)
{
	stream::document jsub;
	MakeBsonMap(request.reqMap(), jsub, request.sectbl().name() + ".$.");
	jbuild << "$set" << jsub;
	jsub.clear();
}

void MongoDb::MakeInsertLst(sDBRequest& request, stream::document& jbuild)
{
	stream::document jadd, jsub;
	MakeBsonMap(request.reqMap(), jsub);
	if (request.sectbl().limit() != 0)
	{
		stream::document jPush;
		stream::array arr;
		arr << jsub;
		jPush << "$each" << arr;
		jPush << "$slice" << request.sectbl().limit();
		jadd << request.sectbl().name() << jPush;
	}
	else
		jadd << request.sectbl().name() << jsub;

	jbuild << "$push" << jadd;
	
	jadd.clear();
	jsub.clear();
}

void MongoDb::MakeInsertLogLst(sDBRequest& request, stream::document& jbuild)
{
	stream::document jadd, jsub;
	MakeBsonMap(request.reqMap(), jsub);
	jadd << request.sectbl().name() << jsub;
	jbuild << "$push" << jadd;
	jadd.clear();
	jsub.clear();
}

void MongoDb::MakeBsonFields(sDBReqBase& request, stream::document& jbuild)
{
	for (const auto& iter : request.Fields())
	{
		jbuild << iter << true;
	}
	if (request.Fields().size() == 0 && request.sectbl().IsList())
		jbuild << request.sectbl().name() + ".$" << true;
}

void MongoDb::MakeBsonMap(sDBMap& con, stream::document& jbuild, const std::string& strIndex)
{
	MakeBson(con.MapNum(), jbuild, strIndex);
	MakeBson(con.MapStr(), jbuild, strIndex);
	MakeBson(con.MapList(), jbuild, strIndex);
	MakeBson(con.MapListStr(), jbuild, strIndex);
	MakeBson(con.MapDouble(), jbuild, strIndex);
}

size_t MongoDb::MakeKeyBson(sDBMap& con, const std::string& key, const std::string& keyEx
	, stream::document& jbuild, bool bErase)
{
	auto iternum = con.MapNum().find(key);
	if (iternum != con.MapNum().end())
	{
		if (iternum->second.type == eDIT_Int)
			AppendNum(jbuild, keyEx, iternum->second.value, iternum->second.caltype);
		else if (iternum->second.type == eDIT_Time)
			AppendTime(jbuild, keyEx, iternum->second.value, iternum->second.caltype);

		if (bErase) con.MapNum().erase(iternum);
		return 1;
	}

	auto iterstr = con.MapStr().find(key);
	if (iterstr != con.MapStr().end())
	{
		if (iterstr->second.type == eDIT_Str)
			AppendStr(jbuild, keyEx, iterstr->second.value, iterstr->second.caltype);
		else if (iterstr->second.type == eDIT_Binary)
			AppendBinary(jbuild, keyEx, iterstr->second.value, iterstr->second.caltype);

		if (bErase) con.MapStr().erase(iterstr);
		return 1;
	}

	auto iterlst = con.MapList().find(key);
	if (iterlst != con.MapList().end())
	{
		if (iterlst->second.size() == 0)
			return 0;

		size_t ret = iterlst->second.size();
		MakeBsonCon(iterlst->second, jbuild, keyEx);

		if (bErase) con.MapList().erase(iterlst);
		return ret;
	}
	return 0;
}

void MongoDb::DoError(const std::string& funcName, int lineno)
{
	CheckConnect();
	sDBRequest stRequest(eDB_InsertLog, "u_error_log");
	stRequest.AddReqStr("exception", _strException);
	stRequest.AddReq("errorno", _errorno);
	stRequest.AddReqStr("query", _ssQueryLog.str());
	stRequest.AddReqTime("time", GetCurrTime());
	stRequest.AddReqStr("tablename", _tblName); 
	stRequest.AddReqStr("sectablename", _secTblName);
	stRequest.AddReqStr("function", funcName);
	stRequest.AddReq("line", lineno);
	stRequest.AddReqStr("proc", get_exec_name());
	try
	{
		stream::document jbuild;
		MakeBsonMap(stRequest.reqMap(), jbuild);
		_insert(stRequest.tbl(), jbuild);
		jbuild.clear();
	}
	catch (std::exception& e)
	{
		Log_Error("%s,%s : %s", stRequest.tbl().c_str(), stRequest.sectbl().name().c_str(), e.what());
		SetErr(e.what());
	}
}