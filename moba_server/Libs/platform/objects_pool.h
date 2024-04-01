#pragma once

template<class T>
class ObjectsPool
{
private:
	struct object_ele_t
	{
		T element;							//成员
		object_ele_t* pNext = nullptr;
		object_ele_t* pPre = nullptr;
		bool bAlloc = false;
		bool bFirst = true;
	};

	struct object_set_t
	{
		object_ele_t* pCurSet = nullptr;
		object_set_t* pNextSet = nullptr;
	};
public:
	ObjectsPool(void) : m_pObjSet(nullptr), m_pUnusedSet(nullptr) { }
	virtual ~ObjectsPool() { }
private:
	object_set_t* m_pObjSet = nullptr;
	object_ele_t* m_pUnusedSet = nullptr;

	int m_iAllocNum = 0;
	int m_iEleNum = 0;
	int m_iSetNum = 0;
	bool m_bNewEvery = true;
public:
	virtual bool Create(int iEleNum, bool bNewEvery = true)
	{
		m_iEleNum = iEleNum;
		m_iSetNum = 1;
		m_bNewEvery = bNewEvery;
		m_pObjSet = new object_set_t;
		m_pObjSet->pNextSet = nullptr;
		m_pObjSet->pCurSet = new object_ele_t[m_iEleNum];

		//关联对象成员
		for (int i = 0; i < m_iEleNum; i++)
		{
			if (i > 0)
			{
				m_pObjSet->pCurSet[i].pPre = &m_pObjSet->pCurSet[i - 1];
				m_pObjSet->pCurSet[i - 1].pNext = &m_pObjSet->pCurSet[i];
			}
			m_pObjSet->pCurSet[i].bAlloc = false;
		}
		//第一个前指针和最后一个后指针置为空
		m_pObjSet->pCurSet[0].pPre = nullptr;
		m_pObjSet->pCurSet[m_iEleNum - 1].pNext = nullptr;

		m_pUnusedSet = m_pObjSet->pCurSet;
		m_iAllocNum = 0;
		return true;
	}

	virtual void Destroy()
	{
		while (m_pObjSet)
		{
			if (m_pObjSet->pCurSet)
			{
				delete[] m_pObjSet->pCurSet;
				m_pObjSet->pCurSet = nullptr;
			}

			object_set_t* pOld = m_pObjSet;
			m_pObjSet = m_pObjSet->pNextSet;
			delete pOld;
		}
	}

	virtual T* Alloc()
	{
		if (!m_pUnusedSet)
		{
			object_set_t* pSet = new object_set_t;
			pSet->pNextSet = m_pObjSet;
			pSet->pCurSet = new object_ele_t[m_iEleNum];

			//关联对象成员
			for (int i = 0; i < m_iEleNum; i++)
			{
				if (i > 0)
				{
					pSet->pCurSet[i].pPre = &pSet->pCurSet[i - 1];
					pSet->pCurSet[i - 1].pNext = &pSet->pCurSet[i];
				}
				pSet->pCurSet[i].bAlloc = false;
			}
			//第一个前指针和最后一个后指针置为空
			pSet->pCurSet[0].pPre = nullptr;
			pSet->pCurSet[m_iEleNum - 1].pNext = nullptr;

			m_pUnusedSet = pSet->pCurSet;
			m_pObjSet = pSet;
			m_iSetNum++;
		}

		object_ele_t* pFreeObj;
		//获取当前可用的
		pFreeObj = m_pUnusedSet;
		//将下一个设置为可用
		m_pUnusedSet = m_pUnusedSet->pNext;
		if (m_pUnusedSet)
			m_pUnusedSet->pPre = nullptr;
		//将当前可用的占用
		pFreeObj->bAlloc = true;
		m_iAllocNum++;
		T* p = &(pFreeObj->element);
		if (!pFreeObj->bFirst)
		{
			if (!m_bNewEvery)
				pFreeObj->element.Init();
			else
			{
				p->~T();
				p = new (p)T;
			}
		}
		return p;
	}

	void Free(T* pElement)
	{
		object_ele_t* pObject = (object_ele_t*)pElement;
		if (!pObject->bAlloc)
			return;

		if (!m_bNewEvery)
			pObject->element.Release();

		pObject->pNext = m_pUnusedSet;
		pObject->pPre = nullptr;
		if (m_pUnusedSet)
			m_pUnusedSet->pPre = pObject;
		m_pUnusedSet = pObject;
		m_iAllocNum--;

		pObject->bAlloc = false;
		pObject->bFirst = false;
	}

	int GetAllocatedSize()
	{
		return m_iAllocNum;
	}

	int GetElemSize()
	{
		return m_iEleNum;
	}

	int GetPoolSize()
	{
		return m_iEleNum * m_iSetNum;
	}

	bool IsCreated()
	{
		return m_pUnusedSet != nullptr;
	}
};