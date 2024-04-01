#pragma once

#include "mongocxx/client.hpp"
#include "mongocxx/uri.hpp"
#include "mongocxx/instance.hpp"
#include "bsoncxx/builder/stream/document.hpp"
#include "bsoncxx/json.hpp"
#include "bsoncxx/builder/stream/array.hpp"
#include "mongocxx/exception/operation_exception.hpp"
#include <bsoncxx/stdx/optional.hpp>
#include <thread>
#include <mutex>
#include "log_mgr.h"
#include "db_struct.h"
using namespace bsoncxx::builder::stream;
using namespace bsoncxx::builder;

using optx = bsoncxx::stdx::optional<bsoncxx::document::value>;
using fn_RetJsonCall = std::function<bool(const bsoncxx::document::view&)>;

//循环60次,防止连接问题导致不能操作
#define FORDO_EXEC_BEGIN \
for (int i = 0; i < 60; ++i)\
{\
    try\
    {   

#define FORDO_EXEC_END(excep) \
        break;\
    }\
    catch (const excep& err)\
    {\
        if(IsReconCode(err))\
        {\
            Log_Error("reconnect ts:%d", i + 1); \
            if (i >= 59) throw err; \
            std::this_thread::sleep_for(std::chrono::milliseconds(1000)); \
        }\
        else\
        {\
            throw err;\
            break;\
        }\
        if(i < 60) continue; \
    }\
    catch (const std::system_error& err)\
    {\
        Log_Error("error: %s", err.what()); \
        if (i < 60) continue; \
    }\
    catch (const std::exception& err)\
    {\
        Log_Error("error: %s", err.what()); \
        if (i < 60) continue; \
    }\
}

#define BEGIN_TRY_MD try

#define END_TRY_MD(qer1, qer2) \
    catch (const mongocxx::operation_exception& err)\
    {\
        AddQueryLog(qer1);\
        AddQueryLog(qer2);\
        SetErr(err);\
        Log_Error("caught table:%s,%s: %s, code:%d, typename:%s, qer:%s"\
            , _tblName.c_str(), _secTblName.c_str(), err.what()\
            , err.code().value(), typeid(err).name(), _ssQueryLog.str().c_str());\
        DoError(__FUNCTION__, __LINE__);\
        return false;\
    }\
    catch (const std::system_error& err)\
    {\
        AddQueryLog(qer1); \
        AddQueryLog(qer2); \
        SetErr(err.what(), err.code().value()); \
        Log_Error("caught table:%s,%s: %s, code:%d, typename:%s, qer:%s"\
             , _tblName.c_str(), _secTblName.c_str(), err.what()\
             , err.code().value(), typeid(err).name(), _ssQueryLog.str().c_str()); \
        DoError(__FUNCTION__, __LINE__); \
        return false; \
    }\
    catch (const std::exception& err)\
    {\
        AddQueryLog(qer1); \
        AddQueryLog(qer2); \
        SetErr(err.what()); \
        Log_Error("caught table:%s,%s: %s, typename:%s, qer:%s"\
            , _tblName.c_str(), _secTblName.c_str(), err.what()\
            , typeid(err).name(), _ssQueryLog.str().c_str()); \
        DoError(__FUNCTION__, __LINE__); \
        return false; \
    }\


class MongoBase
{
public:
    MongoBase();
    virtual ~MongoBase();

    static void initialize(uint32 areano);
    //static void shutdown();

    virtual bool Init(const std::string& hostname, const std::string& user
        , const std::string& pwd, const std::string& dbname, const std::string& auth
        , bool bAddArea = true);

    virtual bool InitDirect(const std::string& hostname, const std::string& user
        , const std::string& pwd, const std::string& dbname, const std::string& auth
        , uint32_t dirAreano);

    //检测是否可以连接
    bool CheckConnect();
    //与db连接心跳检测
    bool DbPing();

    const std::string& GetLastException() const { return _strException; }
    int32 GetErrorNo() const { return _errorno; }
    void ClearErr();
    void SetErr(const std::string& err, int32 errorno = 0);
    void SetErr(const mongocxx::operation_exception& err);

    static std::string MakeOID();
    virtual void DoError(const std::string& funcname, int lineno) = 0;
    //获取mongo连接
    mongocxx::collection* GetCollection(const std::string& colname);
    //判断mongo连接
    bool HasCollection(const std::string& colname);
    
    static uint32 GetAreano() { return _areano; }

protected:
    void AddQueryLog(const std::string& query);
    void AddQueryLog(const stream::document& query);
    std::string GetQueryLog();

    virtual void LogName(const std::string& tblName, const std::string& secTable = "");

    int64 ReadInt(bsoncxx::document::view vew, const char* szfield);
    std::string ReadStr(bsoncxx::document::view vew, const char* szfield);
    std::string ReadBinary(bsoncxx::document::view vew, const char* szfield);
    std::string ReadOID(bsoncxx::document::view vew, const char* szfield);
    bool IsReconCode(const mongocxx::operation_exception& err);
protected:
    virtual bool _drop_collection(const std::string& ns);
    virtual bool _update_one(const std::string& ns, stream::document& query
        , stream::document& obj, bool upsert = false, bool* pMatch = nullptr);
    virtual bool _update_many(const std::string& ns, stream::document& query
        , stream::document& obj, bool upsert = false);
    virtual optx _find_one(const std::string& ns, stream::document& query
        , stream::document* fieldsToReturn);
    virtual bool _is_exsit(const std::string& ns, stream::document& query
        , stream::document* fieldsToReturn);
    virtual bool _insert(const std::string& ns, stream::document& query);
    virtual bool _insert(const std::string& ns, const bsoncxx::document::view& query);
    mongocxx::cursor _find_many(const std::string& ns,
        stream::document& query, int iReturn, int iSkip = 0,
        stream::document* fieldsToReturn = nullptr, stream::document* pSortdoc = nullptr);
    virtual std::int64_t _count(const std::string& ns, stream::document& query);
    virtual bool _remove(const std::string& ns, stream::document& query, bool justOne, bool bEmptyErr);
    optx _find_and_modify(const std::string& ns, stream::document& query,
        stream::document& update, bool upsert = false, stream::document* fieldsToReturn = nullptr);

protected:
    void MakeBson(std::map<std::string, sDBMap::sDBItemStr>& mapStr, stream::document& jbuild
        , const std::string& strIndex = "", const std::string& exField = "");
    void MakeBson(std::map<std::string, sDBMap::sDBItemNum>& mapInt, stream::document& jbuild
        , const std::string& strIndex = "", const std::string& exField = "");
    void MakeBson(std::map<std::string, double>& mapInt, stream::document& jbuild
        , const std::string & strIndex = "", const std::string& exField = "");
    void MakeBson(std::map<std::string, bool>& mapBool, stream::document& jbuild
        , const std::string& strIndex = "", const std::string& exField = "");
    template <typename K_TYPE>
    void MakeBson(std::map<std::string, std::list<K_TYPE>>& mapArray, stream::document& jbuild
        , const std::string& strIndex = "");
    template <typename K_TYPE>
    void MakeBsonCon(std::map<std::string, std::list<K_TYPE>>& mapArray, stream::document& jbuild
        , const std::string& strIndex = "");
    template <typename K_TYPE>
    void MakeBsonCon(std::list<K_TYPE>& mapArray, stream::document& jbuild
        , const std::string& strKey);

    void AppendNum(stream::document& jbuild, const std::string& fieldName, const int64& value, eDBCalType eCalType);
    void AppendTime(stream::document& jbuild, const std::string& fieldName, const int64& value, eDBCalType eCalType);
    void AppendStr(stream::document& jbuild, const std::string& fieldName, const std::string& value, eDBCalType eCalType);
    void AppendDouble(stream::document& jbuild, const std::string& fieldName, const double& value);
    void AppendBool(stream::document& jbuild, const std::string& fieldName, const bool& value);
    void AppendBinary(stream::document& jbuild, const std::string& fieldName, const std::string& value, eDBCalType eCalType);
    void AppendBinary(stream::document& jbuild, const std::string& fieldName, const bsoncxx::types::b_binary& value, eDBCalType eCalType);
    void MakeArray(const std::list<int64>& lst, bsoncxx::builder::stream::array& arr);
    void MakeArray(const std::list<std::string>& lst, bsoncxx::builder::stream::array& arr);
protected:
    static mongocxx::instance                   instx;
    std::string                                 _szurl;
    mongocxx::client                            _connx;
    mongocxx::database                          _dbx;
    mongocxx::uri                               _urlx;
    std::map<std::string, mongocxx::collection> _mapCols;

    std::string                                 _hostname;              //ip地址
    std::string                                 _user;                  //用户名
    std::string                                 _pwd;                   //密码
    std::string                                 _dbname;                //指定数据库(分服id)
    std::string                                 _auth;                  //认证方式
    bool                                        _bInit = false;         //是否初始化

    //log使用,记录当前操作得数据表,不要在外面赋值
    std::string                                 _tblName;
    std::string                                 _secTblName;
    std::stringstream                           _ssQueryLog;
    //错误记录
    std::string                                 _strException;
    int32                                       _errorno = 0;
    static uint32                               _areano;
};

template <typename K_TYPE>
void MongoBase::MakeBson(std::map<std::string, std::list<K_TYPE>>& mapArray, stream::document& jbuild
    , const std::string& strIndex)
{
    stream::document jin;
    bsoncxx::builder::stream::array arr;

    for (auto iter = mapArray.begin(); iter != mapArray.end(); ++iter)
    {
        if (iter->second.size() == 0) continue;

        jin.clear();
        arr.clear();
        MakeArray(iter->second, arr);
        jin << "$in" << arr;
        if (strIndex.empty())
            jbuild << iter->first << jin;
        else
            jbuild << (strIndex + iter->first) << jin;
    }
}

template <typename K_TYPE>
void MongoBase::MakeBsonCon(std::map<std::string, std::list<K_TYPE>>& mapArray, stream::document& jbuild
    , const std::string& strIndex)
{
    bsoncxx::builder::stream::array arr;
    for (auto iter = mapArray.begin(); iter != mapArray.end(); ++iter)
    {
        if (iter->second.size() == 0) continue;
        arr.clear();
        MakeArray(iter->second, arr);
        if (strIndex.empty())
            jbuild << iter->first << arr;
        else
            jbuild << (strIndex + iter->first) << arr;
    }
}

template <typename K_TYPE>
void MongoBase::MakeBsonCon(std::list<K_TYPE>& mapArray, stream::document& jbuild
    , const std::string& strKey)
{
    stream::document jin;
    bsoncxx::builder::stream::array arr;

    MakeArray(mapArray, arr);
    jin << "$in" << arr;
    jbuild << strKey << jin;
}