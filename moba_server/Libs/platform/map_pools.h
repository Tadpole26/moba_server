#pragma once

#include <vector>
#include "objects_pool.h"

using namespace std;
template <typename F, typename T, size_t NUM_M, size_t MOD_NUM = 1>
class MapPools
{
public:
	MapPools()
	{
		_memPool.Create(50);
	}
	~MapPools()
	{
		Clear();
		_memPool.Destroy();
	}

	size_t GetIndex(F iIndex)
	{
		size_t i = (iIndex / MOD_NUM) % NUM_M;
		return i;
	}

	T* Insert(F iIndex)
	{
		size_t i = GetIndex(iIndex);
		T* p = _memPool.Alloc();
		if (p == nullptr)
			return nullptr;
		(_hashmap[i])[iIndex] = p;
		return p;
	}

	bool Exist(F iIndex)
	{
		size_t i = GetIndex(iIndex);
		return _hashmap[i].find(iIndex) != _hashmap[i].end();
	}

	void Erase(F iIndex)
	{
		size_t i = GetIndex(iIndex);
		auto iter = _hashmap[i].find(iIndex);
		if (iter != _hashmap[i].end())
		{
			_memPool.Free(iter->second);
			_hashmap[i].erase(iter);
		}
	}

	void Free(T* pData)
	{
		_memPool.Free(pData);
	}

	T* Find(F iIndex)
	{
		size_t i = GetIndex(iIndex);
		auto iter = _hashmap[i].find(iIndex);
		if (iter != _hashmap[i].end())
			return iter->second;
		return nullptr;
	}

	void Clear()
	{
		for (size_t i = 0; i < NUM_M; ++i)
		{
			for (auto iter = _hashmap[i].begin(); iter != _hashmap[i].end(); ++iter)
			{
				_memPool.Free(iter->second);
			}
			_hashmap[i].clear();
		}
	}

	const std::map<F, T*> GetMap(size_t iIndex)
	{
		return _hashmap[iIndex];
	}

	const std::map<F, T*> TickMap()
	{
		size_t iIndex = _pos % NUM_M;
		_pos = iIndex + 1;
		return _hashmap[iIndex];
	}

	void GetKey(vector<F>& vecIndex)
	{
		for (size_t i = 0; i < NUM_M; ++i)
		{
			for (auto iter = _hashmap[i].begin(); iter != _hashmap[i].end(); ++iter)
				vecIndex.push_back(iter->first);
		}
	}

	size_t Size()
	{
		size_t uiSize = 0;
		for (size_t i = 0; i < NUM_M; ++i)
			uiSize += _hashmap[i].size();
		return uiSize;
	}

	size_t IndexNum()
	{
		return NUM_M;
	}

	size_t PoolSize()
	{
		return _memPool.GetPoolSize();
	}
private:
	std::map<F, T*> _hashmap[NUM_M];
	ObjectsPool<T> _memPool;
	size_t _pos = 0;
};
