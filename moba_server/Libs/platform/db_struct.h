#pragma once

#include <map>
#include <string>
#include "dtype.h"
#include <functional>
#include <set>
#include <list>
#include <chrono>
#include <vector>

class sDBRet;
class sDBSelectReq;
class sDBRequest;
class sDBMap;

using fn_RetCall = std::function<bool(sDBRet&)>;
using fn_RetSqlCall = std::function<bool(sDBMap&)>;

//���ݿ�ִ������
enum eDBQueryType
{
	eDB_None = -1,
	eDB_Select = 0,
	eDB_Insert = 2,
	eDB_Update = 3,
	eDB_Delete = 4,
	eDB_DropParam = 5,	//ɾ��һ���б�õ����ֶ�
	eDB_InsertUpdate = 6,
	eDB_InsertLog = 7,	//��־ר��
	eDB_IncValue = 8,//��Ե��ֶ�����(����),�Լ�(����)
	eDB_UpdateMany = 9,//����,���ڸ��������˵�����״̬
	eDB_String = 10,	//����ִ����װ�õ��ַ���
	eDB_DropTable = 11, //ɾ����
	eDB_IncUpdate = 12,	//���º������Լ�,dk����
};

//���ݿ��ֶεıȽϲ�ѯ,ֻ֧��һ�����ֶ�
enum eDBCalType
{
	EDCT_EQ = 0,	//����
	EDCT_GT = 1,	//����
	EDCT_LT = 2,	//С��
	EDCT_GTE = 3,	//���ڵ���
	EDCT_LTE = 4,	//С�ڵ���
	EDCT_NE = 5,	//������
	EDCT_END = 6,
};

enum eDBItemType
{
	eDIT_No = 0,
	eDIT_Int = 1,
	eDIT_Time = 2,
	eDIT_Str = 3,
	eDIT_Binary = 4,
	eDIT_Double = 5,
	eDIT_List = 6,
	eDIT_Bool = 7,
};

class sDBMap
{
public:
	struct sDBItemNum
	{
		sDBItemNum()
		{
		}

		sDBItemNum(eDBItemType eType, int64_t llValue)
		{
			type = eType;
			value = llValue;
		}
		eDBItemType type = eDIT_Int;
		int64_t value = 0;
		eDBCalType caltype = EDCT_EQ;
	};

	struct sDBItemStr
	{
		sDBItemStr()
		{
		}

		sDBItemStr(eDBItemType eType, std::string strValue)
		{
			type = eType;
			value = strValue;
		}

		eDBItemType type = eDIT_Str;
		std::string value;
		eDBCalType caltype = EDCT_EQ;
	};

protected:
	std::map<std::string, sDBItemStr> _strs;
	std::map<std::string, sDBItemNum> _nums;
	std::map<std::string, double> _dbfs;
	std::map<std::string, std::list<int64>> _lists;
	std::map<std::string, std::list<std::string>> _liststr;
	std::map<std::string, bool> _bools;

public:
	sDBMap() {}
	~sDBMap() {}

	bool HasField(const std::string& strFiled);
	eDBItemType GetFieldType(const std::string& strFiled);

	void clear();
	bool empty();

	std::map<std::string, sDBItemNum>& MapNum() { return _nums; }
	std::map<std::string, sDBItemStr>& MapStr() { return _strs; }
	std::map<std::string, double>& MapDouble() { return _dbfs; }
	std::map<std::string, std::list<int64>>& MapList() { return _lists; }
	std::map<std::string, std::list<std::string>>& MapListStr() { return _liststr; }
	std::map<std::string, bool>& MapBool() { return _bools; }

	//����
	void AddBinary(const std::string& key, const std::string& value, eDBCalType eCalType = EDCT_EQ);
	template <typename Iterator>
	void AddList(const std::string& key, const Iterator& begin, const Iterator& end)
	{
		auto iter = _lists.find(key);
		if (iter != _lists.end())
		{
			iter->second.clear();
			iter->second.assign(begin, end);
		}
		else
		{
			std::list<int64> lstItem(begin, end);
			_lists.emplace(std::make_pair(key, lstItem));
		}
	}
	template <typename Iterator>
	void AddListStr(const std::string& key, const Iterator& begin, const Iterator& end)
	{
		auto iter = _liststr.find(key);
		if (iter != _liststr.end())
		{
			iter->second.clear();
			iter->second.assign(begin, end);
		}
		else
		{
			std::list<std::string> lstItem(begin, end);
			_liststr.emplace(std::make_pair(key, lstItem));
		}
	}
	void AddDouble(const std::string& key, const double& value, eDBCalType eCalType = EDCT_EQ);
	void AddStr(const std::string& key, const std::string& value, eDBCalType eCalType = EDCT_EQ);
	template <typename T>
	void Add(const std::string& key, const T& value, eDBCalType eCalType = EDCT_EQ)
	{
		auto iter = _nums.find(key);
		if (iter != _nums.end())
		{
			iter->second.type = eDIT_Int;
			iter->second.value = (int64_t)value;
			iter->second.caltype = eCalType;
		}
		else
		{
			sDBItemNum num;
			num.type = eDIT_Int;
			num.value = value;
			num.caltype = eCalType;
			_nums.emplace(std::make_pair(key, num));
		}
	}
	void AddTime(const std::string& key, const time_t& value, eDBCalType = EDCT_EQ);
	void AddTime(const std::string& key, const std::chrono::seconds& value, eDBCalType = EDCT_EQ);
	void AddBool(const std::string& key, const bool& value, eDBCalType eCalType = EDCT_EQ);

	//��ȡ
	const std::list<int64>& List(const std::string& key, const std::list<int64>& def = {})
	{
		auto iter = _lists.find(key);
		if (iter != _lists.end())
			return iter->second;

		return def;
	}
	const std::list<std::string>& ListStr(const std::string& key, const std::list<std::string>& def = {})
	{
		auto iter = _liststr.find(key);
		if (iter != _liststr.end())
			return iter->second;
		return def;
	}
	const std::string& Str(const std::string& key, const std::string& def = "");
	const std::string& Binary(const std::string& key, const std::string& def = "");
	const time_t Time(const std::string& key, const time_t& def = 0);
	const double Double(const std::string& key, const double& def = 0.0f);
	const float Float(const std::string& key, const float& def = 0.0f);
	template <typename T>
	const T Num(const std::string& key, const T& def = 0)
	{
		auto iter = _nums.find(key);
		if (iter != _nums.end()) return (T)(iter->second.value);
		return def;
	}

	const int32 NumInt(const std::string& key, const int32& def = 0) { return Num<int32>(key, def); }
	const uint32 NumUInt(const std::string& key, const uint32& def = 0) { return Num<uint32>(key, def); }
	const int64 NumLong(const std::string& key, const int64& def = 0) { return Num<int64>(key, def); }
	const uint64 NumULong(const std::string& key, const uint64& def = 0) { return Num<uint64>(key, def); }
	const bool Bool(const std::string& key, const bool& def = false) { return Num<uint32>(key, def) != 0; }
};

using sDBSecRet = std::vector<sDBMap>;

class sDBRet
{
public:
	void clear();
	bool empty() { return _retMain.empty() && _retVec.empty(); }

	//����ֵ
	const std::list<int64>& List(const std::string& key, const std::list<int64>& def = {}) { return _retMain.List(key, def); }
	const std::string& Str(const std::string& key, const std::string& def = "") { return _retMain.Str(key, def); }
	const std::string& Binary(const std::string& key, const std::string& def = "") { return _retMain.Binary(key, def); }
	template <typename T>
	const T Num(const std::string& key, const T& def = 0) { return _retMain.Num(key, def); }
	const int32 NumInt(const std::string& key, const int32& def = 0) { return _retMain.NumInt(key, def); }
	const uint32 NumUInt(const std::string& key, const uint32& def = 0) { return _retMain.NumUInt(key, def); }
	const int64 NumLong(const std::string& key, const int64& def = 0) { return _retMain.NumLong(key, def); }
	const uint64 NumULong(const std::string& key, const uint64& def = 0) { return _retMain.NumULong(key, def); }
	const time_t Time(const std::string& key, const time_t& def = 0) { return _retMain.Time(key, def); }
	const double Double(const std::string& key, const double& def = 0.0f) { return _retMain.Double(key, def); }
	const float Float(const std::string& key, const float& def = 0.0f) { return _retMain.Float(key, def); }
	const bool Bool(const std::string& key, const bool& def = false) { return _retMain.Bool(key, def); }

	//����
	sDBMap& Ret_Main() { return _retMain; }
	sDBSecRet& Ret_Vec(const std::string& key) { return _retVec[key]; }
	std::map<std::string, sDBSecRet>& Ret_Vec() { return _retVec; }
	bool HaveVecField(const std::string& key) { return _retVec.find(key) != _retVec.end(); }
	void AddField(const std::string& key) { _lstFileds.push_back(key); }

protected:
	sDBMap								_retMain;				//����һ���ֶ�
	std::map<std::string, sDBSecRet>	_retVec;				//���ض����ֶ�����
	std::list<std::string>				_lstFileds;				//ÿ���ֶε�˳��
};

class tagSecTblDesc
{
public:
	//���ݲ��������������
	enum eDBSecType
	{
		eST_None,		//�޶���
		eST_Object,		//����Ϊ������
		eST_List,		//����Ϊ�б�
	};

protected:
	friend class sDBReqBase;
	std::string m_secTblName;				//��������
	eDBSecType m_secType = eST_None;		//�����������
	int32 m_secLimit = 0;					//����:�������, ����:����ǰ��

public:
	void clear()
	{
		m_secTblName.clear();
		m_secType = eST_None;
		m_secLimit = 0;
	}

	const std::string& name() { return m_secTblName; }

	eDBSecType type() { return m_secType; }
	bool IsList() { return m_secType == eST_List; }
	bool IsObj() { return m_secType == eST_Object; }
	bool IsEmpty() { return m_secType == eST_None; }

	const int32 limit() { return m_secLimit; }
};

class sDBReqBase
{
public:
	enum eCheckKeyType
	{
		eCKT_KEY = 1,
		eCKT_SKEY = 2,
	};

public:
	const std::string F_KEY = "_id";
	const std::string S_KEY = "id";

	virtual void clear();

	//����
	const std::string& tbl() { return m_tblName; }
	void SetTbl(std::string& strTbl) { m_tblName = strTbl; }
	tagSecTblDesc& sectbl() { return m_secTable; }
	//����:������� ����:����ǰ��
	void SetSecLimit(int32 limit) { m_secTable.m_secLimit = limit; }
	sDBMap& Con() { return m_conditions; }
	eDBQueryType type() { return m_eQueryType; }
	void SetType(eDBQueryType type) { m_eQueryType = type; }
	//����
	void AddConStr(const std::string& key, const std::string& value, eDBCalType eCalType = EDCT_EQ) { m_conditions.AddStr(key, value, eCalType); }
	template <typename T>
	void AddCon(const std::string& key, const T& value, eDBCalType eCalType = EDCT_EQ) { m_conditions.Add(key, value, eCalType); }
	void AddConTime(const std::string& key, const time_t& value, eDBCalType eCalType = EDCT_EQ) { m_conditions.AddTime(key, value, eCalType); }
	template <typename Iterator>
	void AddConLst(const std::string& key, const Iterator& begin, const Iterator& end) { m_conditions.AddList(key, begin, end); }
	//�ֶ�
	void AddField(const char* field, const std::string& strEx = "") { m_vFields.insert(strEx + field); }
	void AddField(const std::set<std::string>& sfield) { m_vFields.insert(sfield.begin(), sfield.end()); }
	void AddObjField(const char* sectable, const std::string& field);
	void AddListField(const char* sectable, const std::string& field);
	std::set<std::string>& Fields() { return m_vFields;}
	//���ò�ѯ�ֶ�
	void reset_field() { m_vFields.clear(); }
	void reset_con() { m_conditions.clear(); }

	void Init(eDBQueryType type, const std::string& strName
		, const std::string& strSecName = "", tagSecTblDesc::eDBSecType secType = tagSecTblDesc::eST_None);
	void InitSecTbl(const std::string& strSecName, tagSecTblDesc::eDBSecType secType = tagSecTblDesc::eST_List);
	void clear_sectbl();

	virtual bool Check(int nCheckType = eCKT_KEY | eCKT_SKEY);
protected:
	tagSecTblDesc m_secTable;
	std::string m_tblName;					//����
	eDBQueryType m_eQueryType;				//ִ������
	sDBMap	m_conditions;					//����
	std::set<std::string> m_vFields;		//���ҡ�ɾ�����ֶ�
};

//����
class sDBSelectReq : public sDBReqBase
{
public:
	sDBSelectReq(const std::string& strName = "",
		const std::string& strSecName = "",
		tagSecTblDesc::eDBSecType secType = tagSecTblDesc::eST_None)
	{
		sDBReqBase::Init(eDB_Select, strName, strSecName, secType);
	}
	virtual ~sDBSelectReq() {}

	void Init(const std::string& strName, const std::string& strSecName = "",
		tagSecTblDesc::eDBSecType secType = tagSecTblDesc::eST_None)
	{
		sDBReqBase::Init(eDB_Select, strName, strSecName, secType);
	}

	virtual void clear()
	{
		sDBReqBase::clear();
		m_retField.clear();
	}

	virtual void clear_ret()
	{
		m_retField.clear();
	}

	//��÷���ֵ
	sDBRet& Ret() { return m_retField; }
	sDBMap& Ret_Main() { return m_retField.Ret_Main(); }
	sDBSecRet& Ret_Vec(const std::string& key) { return ((m_retField.Ret_Vec())[key]); }
	std::map<std::string, sDBSecRet>& Ret_Vec() { return m_retField.Ret_Vec(); }
	//�жϷ���Ϊ��
	bool empty_ret() { return m_retField.empty(); }
	virtual bool Check(int nCheckType = eCKT_KEY | eCKT_SKEY) { return sDBReqBase::Check(nCheckType); }
protected:
	sDBRet			m_retField;					//���صĽ������
};

//�޸ġ�ɾ��������
class sDBRequest : public sDBReqBase
{
public:
	sDBRequest(const sDBRequest& req);
	sDBRequest& operator = (const sDBRequest& req);
	sDBRequest(eDBQueryType eType = eDB_None
		, const std::string& strName = ""
		, const std::string& strSecName = ""
		, tagSecTblDesc::eDBSecType secType = tagSecTblDesc::eST_None)
	{
		Init(eType, strName, strSecName, secType);
	}
	virtual ~sDBRequest() {}

	bool ReqEmpty() { return m_reqFields.empty(); }
	virtual void clear()
	{
		sDBReqBase::clear();
		m_reqFields.clear();
	}

	sDBMap& reqMap() { return m_reqFields; }
	//����
	void AddReqStr(const std::string& key, const std::string& value) { m_reqFields.AddStr(key, value); }
	template <typename T>
	void AddReq(const std::string& key, const T& value) { m_reqFields.Add(key, value); }
	void AddReqDouble(const std::string& key, const double& value) { m_reqFields.AddDouble(key, value); }
	void AddReqTime(const std::string& key, const time_t& value) { m_reqFields.AddTime(key, value); }
	void AddReqBinary(const std::string& key, const std::string& value) { m_reqFields.AddBinary(key, value); }
	//����bool����
	void AddBool(const std::string& key, const bool& value) { m_reqFields.AddBool(key, value); }
	template <typename Iterator>
	void AddReqList(const std::string& key, const Iterator& begin, const Iterator& end) { m_reqFields.AddList(key, begin, end); }
	template <typename Iterator>
	void AddReqListStr(const std::string& key, const Iterator& begin, const Iterator& end) { m_reqFields.AddListStr(key, begin, end); }
	
	virtual bool Check(int nCheckType = eCKT_KEY | eCKT_SKEY) { return sDBReqBase::Check(nCheckType); }

	bool IsDelError() { return m_bDelError; }
	void SetDelError(bool bError) { m_bDelError = bError; }
protected:
	sDBMap m_reqFields;					//�����ֶ�
	bool m_bDelError = true;			//ɾ��������Ϊ��ʱ����
};
