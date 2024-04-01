#include "pch.h"
#include "parse_pb.h"
#include "log_mgr.h"


bool SerToString(const google::protobuf::MessageLite* pMsg, void* pStr, size_t len)
{
	if (pMsg == nullptr || pStr == nullptr)
		return false;

	try
	{
		if (!pMsg->SerializeToArray(pStr, (int)len))
		{
			Log_Error("ser %s proto, fields:%s!", typeid(*pMsg).name()
				, pMsg->InitializationErrorString().c_str());
			return false;
		}
		return true;
	}
	catch (const std::exception& e)
	{
		Log_Error("ser %s proto, fields:%s!, error:%s!", typeid(*pMsg).name()
			, pMsg->InitializationErrorString().c_str(), e.what());
	}
	return false;
}


bool SerToString(const google::protobuf::MessageLite* pMsg, std::string* pStr)
{
	if (pMsg == nullptr || pStr == nullptr)
		return false;

	try
	{
		if (!pMsg->SerializeToString(pStr))
		{
			Log_Error("ser %s proto, fields:%s!", typeid(*pMsg).name()
				, pMsg->InitializationErrorString().c_str());
			return false;
		}
		return true;
	}
	catch (const std::string& e)
	{
		Log_Error("ser %s proto, fields:%s!, error:%s!", typeid(*pMsg).name()
			, pMsg->InitializationErrorString().c_str(), e.c_str());
	}
	return false;
}

bool SerToString(const google::protobuf::MessageLite& oMsg, std::string& oStr)
{
	return SerToString(&oMsg, &oStr);
}

bool ParseFromString(google::protobuf::MessageLite* pMsg, const void* pData, size_t nSize)
{
	if (nullptr == (pData) || pMsg == nullptr)
		return false;

	try
	{
		if (!pMsg->ParseFromArray(pData, (int)nSize))
		{
			Log_Error("parse %s proto error, fields:%s!", typeid(*pMsg).name(), pMsg->InitializationErrorString().c_str());
			return false;
		}
	}
	catch (const std::string& e)
	{
		Log_Error("parse %s proto error, fields:%s!, error:%s!", typeid(*pMsg).name(), pMsg->InitializationErrorString().c_str(), e.c_str());
		return false;
	}
	return true;
}
