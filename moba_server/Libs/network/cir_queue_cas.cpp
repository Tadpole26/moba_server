#include "pch.h"
#include "cir_queue_cas.h"

//初始化消息队列
bool TCirQueueCas::Init(uint32_t max_count)
{
    if (max_count == 0)
        return false;

    _elems = new TElemP[max_count];
    memset(_elems, 0, sizeof(TElemP) * max_count);

    if (_elems == nullptr)
        return false;

    _head = 0;
    _tail = 0;
    _cur_count = 0;
    _max_count = max_count;

    return true;
}

//释放消息队列内存
bool TCirQueueCas::Release()
{
    if (_elems != nullptr)
        delete[] _elems;

    _head = 0;
    _tail = 0;
    _cur_count = 0;
    _max_count = 0;
    _elems = nullptr;
    return true;
}

//将消息push_back到队列中
int TCirQueueCas::Push(const TElemP elem)
{
    if (elem == nullptr)
        return -1;

    if (_cur_count >= _max_count)
        return -1;

    _elems[_tail] = elem;
    _tail = _tail + 1;
    if (_tail >= _max_count)
        _tail = 0;
    ++_cur_count;
    if (_cur_count == 1)
        return 1;

    return 2;
}

void TCirQueueCas::SerPop()
{}

//将消息从队列中pop_from
TCirQueueCas::TElemP TCirQueueCas::Pop()
{
    if (_cur_count <= 0)
        return nullptr;

    TElemP elem = *(_elems + _head);
    if (elem == nullptr)
        return nullptr;
    _elems[_head] = nullptr;
    _head = _head + 1;
    if (_head >= _max_count)
        _head = 0;
    if (_cur_count > 0)
        --_cur_count;
    else
        _cur_count = 0;

    return elem;
}

//队列中消息数量
int32_t TCirQueueCas::Size()
{
    return _cur_count;
}

//队列是否没有消息
bool TCirQueueCas::Empty()
{
    if (_cur_count == 0)
	    return true;
    return false;
}

//队列是否消息已满
bool TCirQueueCas::Full()
{
    if (_cur_count == _max_count)
	    return true;
    return false;
}

//如果满了,将cq清空
bool TCirQueueCas::Clear()
{
    _head = 0;
    _tail = 0;
    _cur_count = 0;

    return true;
}
