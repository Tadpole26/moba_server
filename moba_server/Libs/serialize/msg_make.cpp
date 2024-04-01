#include "pch.h"
#include "msg_make.h"
#include "parse_pb.h"
#include "log_mgr.h"
#include "google/protobuf/message_lite.h"

CMsgMake::CMsgMake() {}
CMsgMake::~CMsgMake() {}

tagMsgHead* CMsgMake::MakeHeadMsg(google::protobuf::Message* pMessage
	, msg_id_t usModule, msg_id_t usProtocol, uint32_t uiSequence
	, ProtoMsg::ResultCode eCode)
{
	if (pMessage == nullptr) return nullptr;
	size_t nSize = pMessage->ByteSizeLong();
	char* pDataHead = nullptr;

	if (!ResetSend(nSize))
		goto ERROR1;
	pDataHead = (char*)(m_strHead.c_str() + NET_HEAD_SIZE);
	if (nSize > 0 && !SerToString(pMessage, pDataHead, nSize))
	{
		m_strHead.clear();
		m_strHead.reserve(0);
		goto ERROR1;
	}
	return MakeHeadMsg(pDataHead, nSize, usModule, usProtocol, uiSequence, eCode);

ERROR1:
	Log_Error("%s, sendlen %s : %d, module: %u, protol: %u", typeid(*this).name()
		, typeid(*pMessage).name(), nSize, usModule, usProtocol);
	return nullptr;
}

tagMsgHead* CMsgMake::MakeHeadMsg(const std::string& strMsg
	, msg_id_t usModule, msg_id_t usProtocol, uint32_t uiSequence
	, ProtoMsg::ResultCode eCode)
{
	return MakeHeadMsg(strMsg.c_str(), strMsg.length(), usModule, usProtocol, uiSequence, eCode);
}

tagMsgHead* CMsgMake::MakeHeadMsg(const char* buf, size_t size
	, msg_id_t usModule, msg_id_t usProtocol, uint32_t uiSequence
	, ProtoMsg::ResultCode eCode)
{
	tagMsgHead* pRet = nullptr;
	if (!ResetSend(size)) goto ERROR1;

	pRet = make_head_msg(buf, size, (char*)m_strHead.c_str()
		, usModule, usProtocol, uiSequence, eCode);
	if (pRet == nullptr)
	{
		m_strHead.clear();
		m_strHead.reserve(0);
		goto ERROR1;
	}
	return pRet;
ERROR1:
	Log_Error("%s, sendlen : %d, module: %u, protol: %u", typeid(*this).name()
		, size, usModule, usProtocol);
	return nullptr;
}

void CMsgMake::ClearMakeCache()
{
	if (m_strHead.size() >= MSG_MAX_LEN)
	{
		m_strHead.clear();
		m_strHead.reserve(0);
	}
}

bool CMsgMake::ResetSend(size_t nSize)
{
	size_t len = NET_HEAD_SIZE + nSize;

	if (m_strHead.size() < len)
		m_strHead.resize(len);

	if (m_strHead.size() < len)
	{
		m_strHead.clear();
		m_strHead.reserve(0);
		return false;
	}
	return true;
}