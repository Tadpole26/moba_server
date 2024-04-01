#pragma once
#include "google/protobuf/message.h"
#include "msg_parser.h"
#include "msg_module.pb.h"
#include "result_code.pb.h"

using namespace ProtoMsg;

extern bool SerToString(const google::protobuf::MessageLite* pMsg, void* pStr, size_t len);
extern bool SerToString(const google::protobuf::MessageLite* pMsg, std::string* pStr);
extern bool SerToString(const google::protobuf::MessageLite& oMsg, std::string& oStr);
extern bool ParseFromString(google::protobuf::MessageLite* pMsg, const void* pData, size_t nSize);


#define PARSE_PTL(oProto, pData, nSize) \
	if (!ParseFromString(&(oProto), (pData), (nSize))) \
		return; \

#define PARSE_PTL_STR(oProto, strBuf) \
	PARSE_PTL((oProto), (strBuf).c_str(), (strBuf).size())

#define PARSE_PTL_HEAD(oProto, head) \
	PARSE_PTL((oProto), (NET_DATA_BUF(head)), (NET_DATA_SIZE(head)))

#define PARSE_PTL_RET(oProto, pData, nSize) \
	if (!ParseFromString(&(oProto), (pData), (nSize))) \
		return false; \

#define PARSE_PTL_STR_RET(oProto, strBuf) \
	PARSE_PTL_RET((oProto), (strBuf).c_str(), (strBuf).size())

#define PARSE_PTL_HEAD_RET(oProto, head) \
	PARSE_PTL_RET((oProto), (NET_DATA_BUF(head)), (NET_DATA_SIZE(head)))

//Õë¶ÔCode_Common_FailureÐÂÔö
#define PARSE_PTL_RET_FAIL(oProto, pData, nSize) \
	if (!ParseFromString(&(oProto), (pData), (nSize))) \
		return Code_Common_Failure; \

#define PARSE_PTL_STR_RET_FAIL(oProto, strBuf) \
	PARSE_PTL_RET_FAIL((oProto), (strBuf).c_str(), (strBuf).size())
	