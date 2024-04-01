#pragma once
#include "logic_interface.h"
#include <string>
#include <list>
/*
client结构
(对于GateServer来说是玩家)
(对于GameServer来说是GateServer/WorldServer等等)
*/

class CLogicThread;
struct tagMsgHead;
class CTcpSession : public CLogicSession
{
protected:
	virtual bool _Send(const tagMsgHead* pMsg);
};

class CTcpReconn : public CReconnSession
{
public:
	static const uint32_t DEF_CACHE_SECOND = 20;

	CTcpReconn(bool bCache = true, uint32_t CacheSec = DEF_CACHE_SECOND);
	~CTcpReconn();

	virtual bool AddCacheMsg(const char* pMsg, uint32_t len);
	virtual void on_connect();

protected:
	virtual bool _Send(const tagMsgHead* pMsg);

private:
	bool _bCacheSend = false;
	uint32_t _CacheSec = DEF_CACHE_SECOND;
	std::list<std::string> _lstCache;
};

