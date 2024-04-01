#pragma once
#include <vector>
#include "dtype.h"
#include "objects_pool.h"

template <typename F, typename T>
class MapPool
{
public:
	MapPool()
	{
		_memPool.Create(10);
	}

	~MapPool()
	{
		clear();
		_memPool.Destroy();
	}

	T* insert(F nIndex)
	{
		T* p = _memPool.Alloc();
		if (p == nullptr) return nullptr;

		_hashmap[nIndex] = p;
		return p;
	}

	bool exsit(F nIndex)
	{
		return _hashmap.find(nIndex) != _hashmap.end();
	}

	void erase(F nIndex)
	{
		auto iter = _hashmap.find(nIndex);
		if (iter != _hashmap.end())
		{
			_memPool.Free(iter->second);
			_hashmap.erase(iter);
		}
	}

	T* find(F nIndex)
	{
		auto iter = _hashmap.find(nIndex);
		if (iter != _hashmap.end())
			return iter->second;
		return nullptr;
	}

	void clear()
	{
		for (auto iter = _hashmap.begin(); iter != _hashmap.end(); ++iter)
		{
			_memPool.Free(iter->second);
		}
		_hashmap.clear();
	}

	const std::map<F, T*>& getmap()
	{
		return _hashmap;
	}

	const std::map<F, T*> tickmap()
	{
		return _hashmap;
	}

	void Free(T* pData)
	{
		_memPool.Free(pData);
	}

	void getkey(std::vector<F>& vecIndex)
	{
		for (auto iter = _hashmap.begin(); iter != _hashmap.end(); ++iter)
			vecIndex.push_back(iter->first);
	}

	size_t size()
	{
		return _hashmap.size();
	}

	size_t pool_size()
	{
		return _memPool.GetPoolSize();
	}
private:
	std::map<F, T*>		_hashmap;
	ObjectsPool<T>		_memPool;
	size_t				_pos = 0;
};
