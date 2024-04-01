#pragma once

#include "singleton.h"
#include "objects_pool.h"
#include "global_define.h"

template <typename T>
class ServerPool
{
public:
	ServerPool()
	{
		m_stServerPool.Create(SERVER_INIT_NUM);
		m_stServerMap.clear();
	}
	~ServerPool()
	{
		for (auto iter = m_stServerMap.begin(); iter != m_stServerMap.end(); ++iter)
		{
			std::map<uint32, T*> pairServer = iter->second;
			for (auto server = pairServer.begin(); server != pairServer.end(); ++server)
			{
				m_stServerPool.Free(server->second);
			}
		}
		m_stServerPool.Destroy();
		m_stServerMap.clear();
	}

	T* GetServer(uint32 uiKind, uint32 uiServerId)
	{
		auto iter = m_stServerMap.find(uiKind);
		if (iter == m_stServerMap.end())
			return nullptr;

		auto iterRet = iter->second.find(uiServerId);
		if (iterRet == iter->second.end())
			return nullptr;

		return iterRet->second;
	}

	void GetServer(uint32 uiKind, std::vector<T*>& stServerVec)
	{
		auto iter = m_stServerMap.find(uiKind);
		if (iter == m_stServerMap.end())
			return;

		for (auto& value : iter->second)
			stServerVec.emplace_back(value.second);
	}

	T* AddServer(uint32 uiKind, uint32 uiServerId)
	{
		auto iter = m_stServerMap.find(uiKind);
		if (iter == m_stServerMap.end())
		{
			if (!m_stServerMap.emplace(uiKind, std::map<uint32, T*>()).second)
				return nullptr;
		}

		T* pServer = m_stServerPool.Alloc();
		if (pServer == nullptr) return nullptr;
		pServer->SetServerKind((SERVER_KIND)uiKind);
		pServer->SetServerId(uiServerId);

		if (!m_stServerMap[uiKind].emplace(uiServerId, pServer).second)
		{
			m_stServerPool.Free(pServer);
			return nullptr;
		}

		return pServer;
	}

	void DelServer(uint32 uiKind, uint32 uiServerId)
	{
		T* pServer = GetServer(uiKind, uiServerId);
		if (pServer == nullptr)
			return;

		m_stServerMap[uiKind].erase(uiServerId);
		m_stServerPool.Free(pServer);
	}

	bool HasServer(uint32 uiKind, uint32 uiServerId)
	{
		return GetServer(uiKind, uiServerId) != nullptr;
	}

	size_t Size()
	{
		size_t uiNum = 0;
		for (auto iter = m_stServerMap.begin(); iter != m_stServerMap.end(); ++iter)
		{
			uiNum += iter->size();
		}
		return uiNum;
	}

	size_t Size(uint32 uiKind)
	{
		auto iter = m_stServerMap.find(uiKind);
		if (iter != m_stServerMap.end())
			return iter->second.size();
		return 0;
	}

	std::map<uint32, T*>& GetServerMap(uint32 uiKind)
	{
		return m_stServerMap[uiKind];
	}

private:
	std::map<uint32, std::map<uint32, T*>> m_stServerMap;
	ObjectsPool<T> m_stServerPool;
};