#include "pch.h"
#include "mongo_base.h"
#include "mongocxx/exception/bulk_write_exception.hpp"
#include "mongocxx/exception/query_exception.hpp"
#include "mongocxx/exception/write_exception.hpp"
#include "mongocxx/exception/error_code.hpp"
#include "mongocxx/exception/server_error_code.hpp"
#include <mongocxx/exception/logic_error.hpp>
#include "mongocxx/exception/operation_exception.hpp"
#include <mongocxx/result/update.hpp>
#include <mongoc.h>
#include "bson.h"


uint32_t MongoBase::_areano = 0;
mongocxx::instance MongoBase::instx{};

MongoBase::MongoBase()
{ 
}

MongoBase::~MongoBase()
{
}

bool MongoBase::IsReconCode(const mongocxx::operation_exception& err)
{
    int nerr = err.code().value();
    switch (nerr)
    {
    case MONGOC_ERROR_STREAM_SOCKET:
    case MONGOC_ERROR_STREAM_CONNECT:
    case MONGOC_ERROR_STREAM_INVALID_TYPE:
    case MONGOC_ERROR_STREAM_INVALID_STATE:
    case MONGOC_ERROR_STREAM_NAME_RESOLUTION:
    case MONGOC_ERROR_STREAM_NOT_ESTABLISHED:
    case MONGOC_ERROR_SERVER_SELECTION_FAILURE:
        return true;
        break;
    default:
        return false;
    }
    return false;
}

void MongoBase::AddQueryLog(const std::string& query)
{
    _ssQueryLog << query << ": ";
}

void MongoBase::AddQueryLog(const stream::document& query)
{
    AddQueryLog(bsoncxx::to_json(query));
}

std::string MongoBase::GetQueryLog()
{
    return _ssQueryLog.str();
}

void MongoBase::LogName(const std::string& tblName, const std::string& secTable)
{
    CheckConnect();
    ClearErr();
    _tblName = tblName;
    _secTblName = secTable;
}

void  MongoBase::initialize(uint32 areano)
{
    _areano = areano;
}

bool MongoBase::Init(const std::string& hostname, const std::string& user, const std::string& pwd
    , const std::string& dbname, const std::string& auth, bool bAddArea)
{
    _hostname = hostname;
    _user = user;
    _pwd = pwd;
    _dbname = dbname;
    _auth = auth;

    if (_areano != 0 && bAddArea)
    {
        _dbname += "_";
        _dbname += std::to_string(_areano);
    }

    char szurl[1024];
    if (_auth.empty())
    {
        sprintf(szurl, "mongodb://%s:%s@%s/admin?serverSelectionTryOnce=false"
            , user.c_str(), pwd.c_str(), hostname.c_str());
    }
    else
    {
        sprintf(szurl, "mongodb://%s:%s@%s/admin?serverSelectionTryOnce=false", user.c_str(), pwd.c_str(), hostname.c_str());
    }
    _szurl = szurl;
    return CheckConnect();
}

bool MongoBase::InitDirect(const std::string& hostname, const std::string& user, const std::string& pwd
    , const std::string& dbname, const std::string& auth, uint32_t dirAreano)
{
    _hostname = hostname;
    _user = user;
    _pwd = pwd;
    _dbname = dbname;
    _auth = auth;
    if (dirAreano != 0)
    {
        _dbname += "_";
        _dbname += std::to_string(dirAreano);
    }
    char szurl[1024];
    if (_auth.empty())
    {
        sprintf(szurl, "mongodb://%s:%s@%s/admin?serverSelectionTryOnce=false"
            , user.c_str(), pwd.c_str(), hostname.c_str());
    }
    else
    {
        sprintf(szurl, "mongodb://%s:%s@%s/admin?authMechanism=%s&serverSelectionTryOnce=false"
            , user.c_str(), pwd.c_str(), hostname.c_str(), _auth.c_str());
    }
    _szurl = szurl;
    return CheckConnect();
}

bool MongoBase::CheckConnect()
{
    if (_bInit) return true;
    try
    {
        _mapCols.clear();

        _urlx = mongocxx::uri{ _szurl };
        _connx = mongocxx::client{ _urlx };
        _dbx = _connx.database(_dbname);

        document docx;
        docx << "getLastError" << 1;
        _dbx.run_command(docx.view());

        _bInit = true;
    }
    catch (const mongocxx::operation_exception& err)
    {
        int nerr = err.code().value();
        Log_Error("%s, code:%d", err.what(), nerr);
        _bInit = false;
        return false;
    }
    catch (std::exception& err)
    {
        Log_Error("%s", err.what());
        _bInit = false;
        return false;
    }
    return _bInit;
}

bool MongoBase::DbPing()
{
    if (!_bInit)
    {
        if (!CheckConnect()) return false;
        return true;
    }

    try
    {
        document docx;
        docx << "getLastError" << 1;
        _dbx.run_command(docx.view());
        return true;
    }
    catch (const mongocxx::operation_exception& err)
    {
        _bInit = false;
        _mapCols.clear();
        int nerr = err.code().value();
        Log_Error("%s, code:%d", err.what(), nerr);
        return false;
    }
    catch (std::exception& err)
    {
        _bInit = false;
        _mapCols.clear();
        Log_Error("%s", err.what());
        return false;
    }
    return true;
}

mongocxx::collection* MongoBase::GetCollection(const std::string& colname)
{
    if (!CheckConnect())
        return nullptr;

    auto itr = _mapCols.find(colname);
    if (itr != _mapCols.end())
        return &(itr->second);
    else
    {
        _mapCols[colname] = _dbx.collection(colname);
        return &_mapCols[colname];
    }
}

bool MongoBase::HasCollection(const std::string& colname)
{
    if (!CheckConnect())
        return false;

    FORDO_EXEC_BEGIN
    return _dbx.has_collection(colname);
    FORDO_EXEC_END(mongocxx::v_noabi::operation_exception)
    return false;
}

void MongoBase::ClearErr()
{
    _ssQueryLog.str("");
    _strException.clear();
    _errorno = 0;
}

void MongoBase::SetErr(const std::string& err, int32 errorno)
{
    _strException = err;
    _errorno = errorno;
}

void MongoBase::SetErr(const mongocxx::operation_exception& err)
{
    //int nerr = err.code().value();
    if (err.raw_server_error())
        _ssQueryLog << bsoncxx::to_json(*(err.raw_server_error()));
    _strException = err.what();
}

std::string MongoBase::MakeOID()
{
    return bsoncxx::oid().to_string();
}

int64 MongoBase::ReadInt(bsoncxx::document::view vew, const char* szfield)
{
    auto citr = vew.find(szfield);
    if (citr == vew.end()) return 0;
    const bsoncxx::document::element& obj = *citr;
    switch (obj.type())
    {
    case bsoncxx::type::k_int32:
        return (int64)obj.get_int32();
        break;
    case bsoncxx::type::k_int64:
        return obj.get_int64();
        break;
    case bsoncxx::type::k_array:
        return (int64)obj.get_double();
        break;
    case bsoncxx::type::k_bool:
        return (int64)obj.get_bool();
        break;
    default:
        break;
    }
    return 0;
}

std::string MongoBase::ReadStr(bsoncxx::document::view vew, const char* szfield)
{
    auto citr = vew.find(szfield);
    if (citr == vew.end()) return "";
    const bsoncxx::document::element& obj = *citr;
    switch (obj.type())
    {
    case bsoncxx::type::k_utf8:
        return obj.get_utf8().value.to_string();
        break;
    default:
        break;
    }
    return "";
}

std::string MongoBase::ReadBinary(bsoncxx::document::view vew, const char* szfield)
{
    auto citr = vew.find(szfield);
    if (citr == vew.end()) return "";
    const bsoncxx::document::element& obj = *citr;
    switch (obj.type())
    {
    case bsoncxx::type::k_binary:
    {
        auto bytes = obj.get_binary();
        std::string ret;
        ret.append((char*)bytes.bytes, bytes.size);
        return ret;
    }
        break;
    default:
        break;
    }
    return "";
}

std::string MongoBase::ReadOID(bsoncxx::document::view vew, const char* szfield)
{
    auto citr = vew.find(szfield);
    if (citr == vew.end()) return "";
    const bsoncxx::document::element& obj = *citr;
    switch (obj.type())
    {
    case bsoncxx::type::k_oid:
        return obj.get_oid().value.to_string();
        break;
    default:
        break;
    }
    return "";
}

bool MongoBase::_drop_collection(const std::string& ns)
{
    FORDO_EXEC_BEGIN
    {
        if (!_dbx.has_collection(ns)) return true;

        mongocxx::collection* col = GetCollection(ns);
        if (col == nullptr) throw std::runtime_error("drop col, col is not exist!!");
        col->drop();
        _mapCols.erase(ns);
    }
    FORDO_EXEC_END(mongocxx::v_noabi::operation_exception);
    return false;
}

bool MongoBase::_update_one(const std::string& ns, stream::document& query
    , stream::document& obj, bool upsert, bool* pMatch)
{
    mongocxx::options::update up{};
    up.upsert(upsert);

    FORDO_EXEC_BEGIN
    {
        mongocxx::collection * col = GetCollection(ns);
        if (col == nullptr) throw std::runtime_error("update one, col is not exist!!");

        bsoncxx::stdx::optional<mongocxx::result::update> ret = col->update_one(query.view(), obj.view(), up);
        if (pMatch != nullptr)
        {
            if (ret == mongocxx::stdx::nullopt)
                *pMatch = false;
            else if ((ret->result().inserted_count() == 0
                && ret->result().modified_count() == 0
                && ret->result().upserted_count() == 0
                && ret->result().matched_count() == 0
                && ret->result().deleted_count() == 0))
            {
                *pMatch = false;
            }
            else
                *pMatch = true;
        }
        else
        {
            if (ret == mongocxx::stdx::nullopt ||
                (ret->result().inserted_count() == 0
                    && ret->result().modified_count() == 0
                    && ret->result().upserted_count() == 0
                    && ret->result().matched_count() == 0
                    && ret->result().deleted_count() == 0))
            {
                DoError(__FUNCTION__, __LINE__);
                Log_Error("UdateOne error : %s-%s-%s", ns.c_str()
                    , bsoncxx::to_json(query).c_str(), bsoncxx::to_json(obj).c_str());
            }
        }
        ret.reset();
        return true;
    }
    FORDO_EXEC_END(mongocxx::v_noabi::bulk_write_exception);
    return false;
}

bool MongoBase::_update_many(const std::string& ns, stream::document& query
    , stream::document& obj, bool upsert)
{
    mongocxx::options::update up{};
    up.upsert(upsert);
    FORDO_EXEC_BEGIN
    {
        mongocxx::collection * col = GetCollection(ns);
        if (col == nullptr) throw std::runtime_error("update many, col is not exist");

        bsoncxx::stdx::optional<mongocxx::result::update> ret = col->update_many(query.view(), obj.view(), up);
        if (ret == mongocxx::stdx::nullopt)
        {
            DoError(__FUNCTION__, __LINE__);
            Log_Error("UdateMany error : %s-%s-%s", ns.c_str()
                , bsoncxx::to_json(query).c_str(), bsoncxx::to_json(obj).c_str());
        }
        ret.reset();
        return true;
    }
    FORDO_EXEC_END(mongocxx::v_noabi::bulk_write_exception);
    return false;
}

optx MongoBase::_find_one(const std::string& ns, stream::document& query
    , stream::document* fieldsToReturn)
{
    if (fieldsToReturn != nullptr)
    {
        mongocxx::options::find fd{};
        fd.projection(fieldsToReturn->view());
        FORDO_EXEC_BEGIN
        {
            mongocxx::collection * col = GetCollection(ns);
            if (col == nullptr) throw std::runtime_error("find one, col is not exist!!");

            return col->find_one(query.view(), fd);
        }
        FORDO_EXEC_END(mongocxx::v_noabi::query_exception);
    }
    else
    {
        FORDO_EXEC_BEGIN
        {
            mongocxx::collection * col = GetCollection(ns);
            if (col == nullptr) throw std::runtime_error("find one, col is not exist!!");

            return col->find_one(query.view());
        }
        FORDO_EXEC_END(mongocxx::v_noabi::query_exception);
    }
    return mongocxx::stdx::nullopt;
}

bool MongoBase::_is_exsit(const std::string& ns, stream::document& query
    , stream::document* fieldsToReturn)
{
    if (fieldsToReturn != nullptr)
    {
        mongocxx::options::find fd{};
        fd.projection(fieldsToReturn->view());
        FORDO_EXEC_BEGIN
        {
            mongocxx::collection * col = GetCollection(ns);
            if (col == nullptr) throw std::runtime_error("find one, col is not exist!!");

            return col->find_one(query.view(), fd) != mongocxx::stdx::nullopt;
        }
        FORDO_EXEC_END(mongocxx::v_noabi::query_exception);
    }
    else
    {
        FORDO_EXEC_BEGIN
        {
            mongocxx::collection * col = GetCollection(ns);
            if (col == nullptr) throw std::runtime_error("find one, col is not exist!!");

            return col->find_one(query.view()) != mongocxx::stdx::nullopt;
        }
        FORDO_EXEC_END(mongocxx::v_noabi::query_exception);
    }
    return false;
}

bool MongoBase::_insert(const std::string& ns, stream::document& query)
{
    FORDO_EXEC_BEGIN
    {
        mongocxx::collection * col = GetCollection(ns);
        if (col == nullptr) throw std::runtime_error("insert, col is not exist");

        bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::result::insert_one> ret = col->insert_one(query.view());
        if (ret == mongocxx::stdx::nullopt ||
            (ret->result().inserted_count() == 0
                && ret->result().modified_count() == 0
                && ret->result().upserted_count() == 0))
        {
            DoError(__FUNCTION__, __LINE__);
            Log_Error("insert error : %s-%s", ns.c_str(), bsoncxx::to_json(query).c_str());
        }
        ret.reset();
        return true;
    }
    FORDO_EXEC_END(mongocxx::v_noabi::bulk_write_exception);
    return false;
}

bool MongoBase::_insert(const std::string& ns, const bsoncxx::document::view& query)
{
    FORDO_EXEC_BEGIN
    {
        mongocxx::collection * col = GetCollection(ns);
        if (col == nullptr) throw std::runtime_error("insert, col is not exist");

        bsoncxx::v_noabi::stdx::optional<mongocxx::v_noabi::result::insert_one> ret = col->insert_one(query);
        if (ret == mongocxx::stdx::nullopt ||
            (ret->result().inserted_count() == 0
                && ret->result().modified_count() == 0
                && ret->result().upserted_count() == 0))
        {
            DoError(__FUNCTION__, __LINE__);
            Log_Error("insert error : %s-%s", ns.c_str(), bsoncxx::to_json(query).c_str());
        }
        ret.reset();
        return true;
    }
    FORDO_EXEC_END(mongocxx::v_noabi::bulk_write_exception);
    return false;
}

mongocxx::cursor MongoBase::_find_many(const std::string& ns,
    stream::document& query, int iReturn, int iSkip,
    stream::document* fieldsToReturn, stream::document* pSortdoc)
{
    mongocxx::options::find fd{};
    if (fieldsToReturn != nullptr)
        fd.projection(fieldsToReturn->view());
    if (iSkip != 0) fd.skip(iSkip);
    if (iReturn != 0) fd.limit(iReturn);
    if (pSortdoc != nullptr) fd.sort(pSortdoc->view());

    mongocxx::collection* col = GetCollection(ns);
    if (col == nullptr) throw std::runtime_error("find all, col is not exist!!");
    return col->find(query.view(), fd);
}

std::int64_t MongoBase::_count(const std::string& ns, stream::document& query)
{
    FORDO_EXEC_BEGIN
    {
        mongocxx::collection * col = GetCollection(ns);
        if (col == nullptr) throw std::runtime_error("count, col is not exist!!");

        int64_t llCount = col->count_documents(query.view());
        return llCount;
    }
    FORDO_EXEC_END(mongocxx::v_noabi::query_exception);
    return 0;
}

bool MongoBase::_remove(const std::string& ns, stream::document& query, bool justOne, bool bEmptyErr)
{
    if (justOne)
    {
        FORDO_EXEC_BEGIN
        {
            mongocxx::collection * col = GetCollection(ns);
            if (col == nullptr) throw std::runtime_error("delete, col is not exist!!");

            auto ret = col->delete_one(query.view());
            if (ret == mongocxx::stdx::nullopt || ret->deleted_count() == 0)
            {
                DoError(__FUNCTION__, __LINE__);
                Log_Error("Delete error : %s-%s", ns.c_str()
                    , bsoncxx::to_json(query).c_str());
            }
            return true;
        }
        FORDO_EXEC_END(mongocxx::v_noabi::bulk_write_exception);
    }
    else
    {
        FORDO_EXEC_BEGIN
        {
            mongocxx::collection * col = GetCollection(ns);
            if (col == nullptr) throw std::runtime_error("delete, col is not exist!!");

            auto ret = col->delete_many(query.view());
            if (ret == mongocxx::stdx::nullopt || ret->deleted_count() == 0)
            {
                DoError(__FUNCTION__, __LINE__);
                Log_Error("Delete error : %s-%s", ns.c_str()
                    , bsoncxx::to_json(query).c_str());
            }
            return true;
        }
        FORDO_EXEC_END(mongocxx::v_noabi::bulk_write_exception);
    }
    return false;
}

optx MongoBase::_find_and_modify(const std::string& ns, stream::document& query,
    stream::document& update, bool upsert, stream::document* fieldsToReturn)
{
    mongocxx::options::find_one_and_update up;
    up.upsert(upsert);
    if (fieldsToReturn != nullptr)
        up.projection(fieldsToReturn->view());

    up.return_document(mongocxx::options::return_document::k_after);
    FORDO_EXEC_BEGIN
    {
        mongocxx::collection * col = GetCollection(ns);
        if (col == nullptr) throw std::runtime_error("collection error!!");

        return col->find_one_and_update(query.view(), update.view(), up);
    }
    FORDO_EXEC_END(mongocxx::v_noabi::write_exception);
    return mongocxx::stdx::nullopt;
}

//////////////////////////////////////////////////////////////////////////////////////////////
std::string szCalType[EDCT_END] = { "$eq", "$gt", "$lt", "$gte", "$lte", "$ne" };

void MongoBase::MakeBson(std::map<std::string, sDBMap::sDBItemStr>& mapStr, stream::document& jbuild
    , const std::string& strIndex , const std::string& exField)
{
    bsoncxx::types::b_binary bytes;
    bytes.sub_type = bsoncxx::binary_sub_type::k_binary;

    for (auto iter = mapStr.begin(); iter != mapStr.end(); ++iter)
    {
        if (exField.size() > 0 && exField == iter->first) continue;

        if (iter->second.type == eDIT_Str)
        {
            if (strIndex.empty())
                AppendStr(jbuild, iter->first, iter->second.value, iter->second.caltype);
            else
                AppendStr(jbuild, (strIndex + iter->first), iter->second.value, iter->second.caltype);
        }
        else if (iter->second.type == eDIT_Binary)
        {
            bytes.bytes = (uint8*)iter->second.value.c_str();
            bytes.size = (uint32_t)iter->second.value.size();
            if (strIndex.empty())
                AppendBinary(jbuild, iter->first, bytes, iter->second.caltype);
            else
                AppendBinary(jbuild, (strIndex + iter->first), bytes, iter->second.caltype);
        }
    }
}

void MongoBase::MakeBson(std::map<std::string, sDBMap::sDBItemNum>& mapInt, stream::document& jbuild
    , const std::string& strIndex, const std::string& exField)
{
    for (auto iter = mapInt.begin(); iter != mapInt.end(); ++iter)
    {
        if (exField.size() > 0 && exField == iter->first) continue;

        if (iter->second.type == eDIT_Int)
        {
            if (strIndex.empty())
                AppendNum(jbuild, iter->first, iter->second.value, iter->second.caltype);
            else 
                AppendNum(jbuild, (strIndex + iter->first), iter->second.value, iter->second.caltype);
        }
        else if (iter->second.type == eDIT_Time)
        {
            if (strIndex.empty())
                AppendTime(jbuild, iter->first, iter->second.value, iter->second.caltype);
            else
                AppendTime(jbuild, (strIndex + iter->first), iter->second.value, iter->second.caltype);
        }
    }
}

void MongoBase::MakeBson(std::map<std::string, double>& mapInt, stream::document& jbuild
    , const std::string& strIndex , const std::string& exField)
{
    for (auto iter = mapInt.begin(); iter != mapInt.end(); ++iter)
    {
        if (exField.size() > 0 && exField == iter->first) continue;
       
        if (strIndex.empty())
            AppendDouble(jbuild, iter->first, iter->second);
        else
            AppendDouble(jbuild, (strIndex + iter->first),iter->second);
    }
}

void MongoBase::MakeBson(std::map<std::string, bool>& mapBool, stream::document& jbuild
    , const std::string& strIndex, const std::string& exField)
{
    for (auto iter = mapBool.begin(); iter != mapBool.end(); ++iter)
    {
        if (exField.size() > 0 && exField == iter->first) continue;

        if (strIndex.empty())
            AppendBool(jbuild, iter->first, iter->second);
        else
            AppendBool(jbuild, (strIndex + iter->first), iter->second);
    }
}


void MongoBase::AppendNum(stream::document& jbuild, const std::string& fieldName, const int64& value, eDBCalType eCalType)
{
    if (eCalType == EDCT_EQ)
    {
        if (value >= INT32_MAX || value <= INT32_MIN)
            jbuild << fieldName << (std::int64_t)value;
        else 
            jbuild << fieldName << (std::int32_t)value;
    }
    else
    {
        stream::document dSort;
        if (value >= INT32_MAX || value <= INT32_MIN)
            dSort << szCalType[eCalType] << (std::int64_t)value;
        else
            dSort << szCalType[eCalType] << (std::int32_t)value;

        jbuild << fieldName << dSort;
    }
}

void MongoBase::AppendTime(stream::document& jbuild, const std::string& fieldName, const int64& value, eDBCalType eCalType)
{
    if (eCalType == EDCT_EQ)
    {
        jbuild << fieldName << bsoncxx::types::b_date(std::chrono::milliseconds(value * 1000));
    }
    else
    {
        stream::document dSort;
        dSort << szCalType[eCalType] << bsoncxx::types::b_date(std::chrono::milliseconds(value * 1000));
        jbuild << fieldName << dSort;
    }

}

void MongoBase::AppendStr(stream::document& jbuild, const std::string& fieldName, const std::string& value, eDBCalType eCalType)
{
    if (eCalType == EDCT_EQ)
        jbuild << fieldName << value;
    else
    {
        stream::document dSort;
        dSort << szCalType[eCalType] << value;
        jbuild << fieldName << dSort;
    }
}

void MongoBase::AppendDouble(stream::document& jbuild, const std::string& fieldName, const double& value)
{
    jbuild << fieldName << value;
}

void  MongoBase::AppendBool(stream::document& jbuild, const std::string& fieldName, const bool& value)
{
    jbuild << fieldName << value;
}

void MongoBase::AppendBinary(stream::document& jbuild, const std::string& fieldName, const std::string& value, eDBCalType eCalType)
{
    bsoncxx::types::b_binary bytes;
    bytes.sub_type = bsoncxx::binary_sub_type::k_binary;
    bytes.bytes = (uint8*)value.c_str();
    bytes.size = (uint32_t)value.size();
    AppendBinary(jbuild, fieldName, bytes, eCalType);
}

void MongoBase::AppendBinary(stream::document& jbuild, const std::string& fieldName, const bsoncxx::types::b_binary& value, eDBCalType eCalType)
{
    if (eCalType == EDCT_EQ)
        jbuild << fieldName << value;
    else
    {
        stream::document dSort;
        dSort << szCalType[eCalType] << value;
        jbuild << fieldName << dSort;
    }
}

void MongoBase::MakeArray(const std::list<int64>& lst, bsoncxx::builder::stream::array& arr)
{
    for (auto& id : lst)
    {
        if (id > INT32_MAX || id <= INT32_MIN)
            arr << (std::int64_t)id;
        else
            arr << (std::int32_t)id;
    }
}

void MongoBase::MakeArray(const std::list<std::string>& lst, bsoncxx::builder::stream::array& arr)
{
    for (auto& id : lst)
        arr << id;
}