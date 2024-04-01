#include "pch.h"
#include "cir_queue_gen.h"

//初始化消息队列
bool TCirQueueGen::Init(uint32_t max_count)
{
    std::unique_lock<std::mutex> lck(_mutex);
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

//释放内存
bool TCirQueueGen::Release()
{
    std::unique_lock<std::mutex> lck(_mutex);

    if (_elems != nullptr)
        delete[] _elems;

    _head = 0;
    _tail = 0;
    _cur_count = 0;
    _max_count = 0;
    _elems = nullptr;
    return true;
}

//将消息push_back到队列中(多线程push_back所以要加锁)
int TCirQueueGen::Push(const TElemP elem)
{
    _mutex.lock();
    if (_cur_count >= _max_count)
    {
        _mutex.unlock();
        return -1;
    }

    _elems[_tail++] = elem;
    if (_tail >= _max_count)
        _tail = 0;

    ++_cur_count;
    if (_cur_count == 1)
    {
        _mutex.unlock();
        return 1;
    }
    _mutex.unlock();
    return 2;
}

void TCirQueueGen::SerPop()
{}

//将消息从队列中pop_from(多个线程pop,所以要加锁)
TCirQueueGen::TElemP TCirQueueGen::Pop()
{
    _mutex.lock();
    if (_cur_count <= 0)
    {
        _mutex.unlock();
        return nullptr;
    }
    TElemP elem = *(_elems + _head);
    _elems[_head] = nullptr;
    ++_head;
    if (_head >= _max_count)
        _head = 0;
    --_cur_count;
    _mutex.unlock();
    return elem;
}

//队列消息数量
int32_t TCirQueueGen::Size()
{
    return _cur_count;
}

//队列是否为空
bool TCirQueueGen::Empty()
{
    if (_cur_count == 0)
        return true;
    return false;
}

//队列是否已满
bool TCirQueueGen::Full()
{
    if (_cur_count == _max_count)
        return true;
    return false;
}

//如果满了,将cq清空
bool TCirQueueGen::Clear()
{
    _head = 0;
    _tail = 0;
    _cur_count = 0;

    return true;
}
