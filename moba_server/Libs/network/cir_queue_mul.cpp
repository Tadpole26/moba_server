#include "pch.h"
#include "cir_queue_mul.h"

//初始化消息队列
bool TCirQueueMul::Init(uint32_t max_count)
{
    std::unique_lock<std::mutex> lck(_mutex);

    if (max_count == 0)
        return false;
    //初始化队列
    _elems = new TElemP[max_count];
    memset(_elems, 0, sizeof(TElemP) * max_count);
    if (_elems == nullptr)
        return false;
    _elemspop = new TElemP[max_count];
    memset(_elemspop, 0, sizeof(TElemP) * max_count);
    if (_elemspop == nullptr)
        return false;

    _tail = 0;
    _cur_count = 0;
    _max_count = max_count;

    return true;
}

//释放内存
bool TCirQueueMul::Release()
{
    std::unique_lock<std::mutex> lck(_mutex);

    if (_elems != nullptr)
        delete[] _elems;
    if (_elemspop != nullptr)
        delete[] _elemspop;

    _tail = 0;
    _cur_count = 0;
    _max_count = 0;
    _elems = nullptr;
    _elemspop = nullptr;
    return true;
}

//将消息push_back到队列中(多线程push_back所以要加锁)
int TCirQueueMul::Push(const TElemP elem)
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
//将push队列消息交换到pop队列中
void TCirQueueMul::SerPop()
{
    _mutex.lock();
    if (_cur_count <= 0)
    {
        _mutex.unlock();
        return;
    }
    std::swap(_elems, _elemspop);
    _pop_count = _cur_count;
    _pop = 0;

    _tail = 0;
    _cur_count = 0;
    _mutex.unlock();
}

//将消息从队列中pop_from(一个线程pop,所以不要加锁)
TCirQueueMul::TElemP TCirQueueMul::Pop()
{
    if (_pop_count <= 0)
        return nullptr;

    TElemP elem = *(_elemspop + _pop);
    _elemspop[_pop] = nullptr;
    ++_pop;

    if (_pop >= _pop_count)
    {
        _pop_count = 0;
        _pop = 0;
    }

    return elem;
}

//push队列消息数量
int32_t TCirQueueMul::Size()
{
    return _cur_count;
}
//push队列是否为空
bool TCirQueueMul::Empty()
{
    return _cur_count == 0;
}
//push队列是否已满
bool TCirQueueMul::Full()
{
    return _cur_count == _max_count;
}

//如果满了，将cq清空
bool TCirQueueMul::Clear()
{
    _tail = 0;
    _cur_count = 0;

    return true;
}
