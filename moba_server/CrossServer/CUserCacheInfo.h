#pragma once
#include "dtype.h"
#include "msg_parser.h"
#include "result_code.pb.h"
#include <string>
using namespace ProtoMsg;

class CUserCacheInfo
{
public:
	CUserCacheInfo();
	~CUserCacheInfo();

	//void Init();
	//void Release();

	//void SendMsg(google::protobuf::Message* pMsg, msg_id_t cModule, msg_id_t cCmd);
	//void SendMsgToCli(google::protobuf::Message* pMsg, msg_id_t cModule, msg_id_t cCmd,
	//	int eCode = ResultCode::Code_Common_Success, bool bPush = true);
public:
	uint32_t m_uiSeq = 0;
	uint32_t m_uiGameId = 0;

	int64_t m_llUid = 0;					//唯一id
	std::string m_strNick = "";				//昵称
	std::string m_strAccName = "";			//sdk唯一账号
	uint32_t	m_uiGender = 0;				//性别
	uint32_t	m_uiLevel = 0;				//等级
};

using user_cache_info_ptr_type = CUserCacheInfo*;