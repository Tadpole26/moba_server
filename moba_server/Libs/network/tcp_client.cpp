#include "pch.h"
#include "tcp_client.h"
#include "msg_parser.h"
#include "log_mgr.h"

bool CTcpSession::_Send(const tagMsgHead* pMsg)
{
	if (m_pDispatcher != nullptr)
		return my_send_conn_msg(m_pDispatcher, m_hd.m_threadOid, m_hd.m_connOid, pMsg);
	else
		return false;
}

CTcpReconn::CTcpReconn(bool bCache, uint32_t CacheSec)
	: _bCacheSend(bCache)
	, _CacheSec(CacheSec)
{
}

CTcpReconn::~CTcpReconn()
{
	_lstCache.clear();
}

bool CTcpReconn::AddCacheMsg(const char* pMsg, uint32_t len)
{
	if (_bCacheSend)
	{
		//只缓存突然断开的,其它时间不保存
		if (GetCurrTime() - m_tmDiscon < _CacheSec)
		{
			_lstCache.push_back(std::string(pMsg, len));
			return true;
		}
		else
			_lstCache.clear();

		if (_lstCache.size() > 1000)
			Log_Error("size:%d, name:%s", _lstCache.size(), typeid(*this).name());
	}
	return false;
}

bool CTcpReconn::_Send(const tagMsgHead* pMsg)
{
	if (is_connected() && m_pDispatcher != nullptr)
		return my_send_conn_msg(m_pDispatcher, m_hd.m_threadOid, m_hd.m_connOid, pMsg);
	else
		return AddCacheMsg((const char*)pMsg, pMsg->uiLen);
}

void CTcpReconn::on_connect()
{
	if (_bCacheSend && m_pDispatcher != nullptr)
	{
		//只缓存突然断开的,其它时间不保存
		if (GetCurrTime() - m_tmDiscon < _CacheSec)
		{
			for (auto itr = _lstCache.begin(); itr != _lstCache.end(); ++itr)
				my_send_conn_msg(m_pDispatcher, m_hd.m_threadOid, m_hd.m_connOid, (tagMsgHead*)(itr->c_str()));
			_lstCache.clear();
		}
	}
}
