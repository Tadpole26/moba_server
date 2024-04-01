#include "pch.h"
#include "db_struct.h"

bool sDBMap::HasField(const std::string& strFiled)
{
	if (_nums.find(strFiled) != _nums.end())
		return true;
	if (_strs.find(strFiled) != _strs.end())
		return true;
	if (_lists.find(strFiled) != _lists.end())
		return true;
	if (_liststr.find(strFiled) != _liststr.end())
		return true;
	if (_dbfs.find(strFiled) != _dbfs.end())
		return true;
	if (_bools.find(strFiled) != _bools.end())
		return true;
	return false;
}

eDBItemType sDBMap::GetFieldType(const std::string& strFiled)
{
	if (_nums.find(strFiled) != _nums.end())
		return eDIT_Int;
	if (_strs.find(strFiled) != _strs.end())
		return eDIT_Str;
	if (_lists.find(strFiled) != _lists.end())
		return eDIT_List;
	if (_liststr.find(strFiled) != _liststr.end())
		return eDIT_List;
	if (_dbfs.find(strFiled) != _dbfs.end())
		return eDIT_Double;
	if (_bools.find(strFiled) != _bools.end())
		return eDIT_Bool;
	return eDIT_No;
}

void sDBMap::clear()
{
	_nums.clear();
	_strs.clear();
	_lists.clear();
	_liststr.clear();
	_dbfs.clear();
	_bools.clear();
}

bool sDBMap::empty()
{
	return _nums.size() == 0 && _strs.size() == 0
		&& _lists.size() == 0 && _liststr.size() == 0
		&& _dbfs.size() == 0 && _bools.size() == 0;
}

void sDBMap::AddBinary(const std::string& key, const std::string& value, eDBCalType eCalType)
{
	auto iter = _strs.find(key);
	if (iter != _strs.end())
	{
		iter->second.type = eDIT_Binary;
		iter->second.value = value;
		iter->second.caltype = eCalType;
	}
	else
	{
		sDBItemStr str;
		str.type = eDIT_Binary;
		str.value = value;
		str.caltype = eCalType;
		_strs.emplace(std::make_pair(key, str));
	}
}

void sDBMap::AddDouble(const std::string& key, const double& value, eDBCalType eCalType)
{
	auto iter = _dbfs.find(key);
	if (iter != _dbfs.end())
	{
		iter->second = value;
	}
	else
	{
		_dbfs.emplace(std::make_pair(key, value));
	}
}

void sDBMap::AddStr(const std::string& key, const std::string& value, eDBCalType eCalType)
{
	auto iter = _strs.find(key);
	if (iter != _strs.end())
	{
		iter->second.type = eDIT_Str;
		iter->second.value = value;
		iter->second.caltype = eCalType;
	}
	else
	{
		sDBItemStr str;
		str.type = eDIT_Str;
		str.value = value;
		str.caltype = eCalType;
		_strs.emplace(std::make_pair(key, str));
	}
}

void sDBMap::AddTime(const std::string& key, const time_t& value, eDBCalType eCalType)
{
	auto iter = _nums.find(key);
	if (iter != _nums.end())
	{
		iter->second.type = eDIT_Time;
		iter->second.value = (int64_t)value;
		iter->second.caltype = eCalType;
	}
	else
	{
		sDBItemNum num;
		num.type = eDIT_Time;
		num.value = value;
		num.caltype = eCalType;
		_nums.emplace(std::make_pair(key, num));
	}
}

void sDBMap::AddTime(const std::string& key, const std::chrono::seconds& value, eDBCalType eCalType)
{
	AddTime(key, value.count(), eCalType);
}

void sDBMap::AddBool(const std::string& key, const bool& value, eDBCalType eCalType)
{
	auto iter = _bools.find(key);
	if (iter != _bools.end())
	{
		iter->second = value;
	}
	else
	{
		_bools.emplace(std::make_pair(key, value));
	}
}

const std::string& sDBMap::Str(const std::string& key, const std::string& def)
{
	auto iter = _strs.find(key);
	if (iter != _strs.end())
		return iter->second.value;
	return def;
}

const std::string& sDBMap::Binary(const std::string& key, const std::string& def)
{
	auto iter = _strs.find(key);
	if (iter != _strs.end())
		return iter->second.value;
	return def;
}

const time_t sDBMap::Time(const std::string& key, const time_t& def)
{
	auto iter = _nums.find(key);
	if (iter != _nums.end())
		return (time_t)(iter->second.value);
	return def;
}

const double sDBMap::Double(const std::string& key, const double& def)
{
	auto iter = _dbfs.find(key);
	if (iter != _dbfs.end())
		return iter->second;
	return def;
}

const float sDBMap::Float(const std::string& key, const float& def)
{
	return (float)Double(key, def);
}

////////////////////////////////////////////////////////////////////////
void sDBRet::clear()
{
	_retMain.clear();
	_retVec.clear();
	_lstFileds.clear();
}
////////////////////////////////////////////////////////////////////////
void sDBReqBase::AddObjField(const char* sectable, const std::string& field)
{
	AddField(field.c_str(), std::string(sectable) + ".");
}

void sDBReqBase::AddListField(const char* sectable, const std::string& field)
{
	AddField(field.c_str(), std::string(sectable) + ".$.");
}

void sDBReqBase::Init(eDBQueryType type, const std::string& strName
	, const std::string& strSecName, tagSecTblDesc::eDBSecType secType)
{
	m_eQueryType = type;
	m_tblName = strName;
	m_secTable.m_secTblName = strSecName;
	m_secTable.m_secType = secType;
	if (m_secTable.m_secType == tagSecTblDesc::eST_None && strSecName.length() > 0)
		m_secTable.m_secType = tagSecTblDesc::eST_List;
}

void sDBReqBase::InitSecTbl(const std::string& strSecName, tagSecTblDesc::eDBSecType secType)
{
	m_secTable.m_secTblName = strSecName;
	if (strSecName.empty())
		m_secTable.m_secType = tagSecTblDesc::eST_None;
	else
		m_secTable.m_secType = secType;
}

void sDBReqBase::clear()
{
	m_conditions.clear();
	m_vFields.clear();
}

void sDBReqBase::clear_sectbl()
{
	m_secTable.clear();
}

bool sDBReqBase::Check(int nCheckType)
{
	if (nCheckType & eCKT_KEY)
	{
		if (m_tblName.empty())
			return true;
		else
		{
			if (!m_conditions.HasField(F_KEY))
				return false;
		}
	}

	if (nCheckType & eCKT_SKEY)
	{
		if (m_secTable.type() == tagSecTblDesc::eST_List)
			return m_conditions.HasField(S_KEY);
	}
	return true;
}
////////////////////////////////////////////////////////////////////////
sDBRequest::sDBRequest(const sDBRequest& req)
{
	m_reqFields = req.m_reqFields;
	m_secTable = req.m_secTable;
	m_tblName = req.m_tblName;
	m_eQueryType = req.m_eQueryType;
	m_conditions = req.m_conditions;
	m_vFields = req.m_vFields;
	m_bDelError = req.m_bDelError;
}

sDBRequest& sDBRequest::operator = (const sDBRequest& req)
{
	m_reqFields = req.m_reqFields;
	m_secTable = req.m_secTable;
	m_tblName = req.m_tblName;
	m_eQueryType = req.m_eQueryType;
	m_conditions = req.m_conditions;
	m_vFields = req.m_vFields;
	m_bDelError = req.m_bDelError;
	return *this;
}
