#pragma once
//idÉú³ÉÆ÷

#include <sstream>
#include <assert.h>

//id·ÖÅäÆ÷
template <typename T, T MAX_VALUE = 0xffffffff>
class CIdAlloctor
{
public:
    CIdAlloctor() : _polds(nullptr), _nall(0), _nalloc(0), _pflags(nullptr)
    {
    }
    ~CIdAlloctor()
    {
        Release();
    }
    bool Init(T nCount, T beginid = 0)
    {
        size_t nNum = (size_t)nCount;
        if (nCount <= 0 || nCount >= MAX_VALUE
            || beginid >= MAX_VALUE || beginid < 0)
            return false;
        _beginid = beginid;
        _nall = nCount;
        _nalloc = 0;
        _polds = new T[nNum];
        for (T i = 0; i < nCount; ++i)
        {
            _polds[i] = i;
        }
        _pflags = new bool[nNum];
        memset(_pflags, 0, sizeof(_pflags[0]) * nNum);
        return true;
    }
    void Release()
    {
        if (_polds != nullptr)
            delete[] _polds;
        if (_pflags != nullptr)
            delete[] _pflags;

        _nall = 0;
        _nalloc = 0;
        _polds = nullptr;
        _pflags = nullptr;
    }
    bool Full()
    {
        return _nalloc == _nall;
    }
    T AllocId()
    {
        T id = MAX_VALUE;
        if (_nalloc < _nall)
        {
            id = _polds[_nalloc++];
            _pflags[id] = true;
            id += _beginid;
        }
        return id;
    }
    void FreeId(T id)
    {
        if (id < _beginid)
            return;
        id = id - _beginid;
        if (id >= _nall)
            return;
        if (_nalloc > 0)
        {
            if (_pflags[id])
            {
                _polds[--_nalloc] = id;
                _pflags[id] = false;
            }
        }
    }
    bool UseId(T id)
    {
        if (id < _beginid)
            return false;
        id = id - _beginid;
        if (id >= _nall)
            return false;
        if (_pflags[id])
            return true;
        else
        {
            if (_nalloc >= _nall)
                return false;

            if (*(_polds + _nalloc) != id)
            {
                T* pRet = std::find(_polds + _nalloc, _polds + _nall - 1, id);
                if (pRet == nullptr || *pRet != id) return false;
                std::swap(*(_polds + _nalloc), *pRet);
            }

            _nalloc++;
            _pflags[id] = true;
            return true;
        }
        return false;
    }

    T Size() { return _nalloc; }
    T MaxValue() { return MAX_VALUE; }

    bool IsValid(T id)
    {
        if (id == MAX_VALUE || id < _beginid)
            return false;
        id = id - _beginid;
        if (id >= _nall)
            return false;
        return true;
    }

    void Dump(std::ostream* pStream)
    {
        std::ostream& stream = *pStream;

        stream << "all:" << _nall << ", "
            << "alloc:" << _nalloc << ", "
            << "free:" << _nall - _nalloc << std::endl;

        stream << "[alloc_list]: ";
        for (T i = 0; i < _nall; ++i)
        {
            if (_pflags[i])
                stream << i << " ";
        }
        stream << std::endl;

        for (T i = _nalloc; i < _nall; ++i)
        {
            assert(!_pflags[_polds[i]]);
        }
        T nCalcFreeCount = 0;
        for (T i = 0; i < _nall; ++i)
        {
            if (!_pflags[i])
                ++nCalcFreeCount;
        }
        assert(nCalcFreeCount == (_nall - _nalloc));
    }

private:
    T*          _polds = nullptr;
    T           _nall = 0;
    T           _nalloc = 0;
    bool*       _pflags = nullptr;
    T           _beginid = 0;
};
