#include "pch.h"
#include "cir_queue_gen.h"

//��ʼ����Ϣ����
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

//�ͷ��ڴ�
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

//����Ϣpush_back��������(���߳�push_back����Ҫ����)
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

//����Ϣ�Ӷ�����pop_from(����߳�pop,����Ҫ����)
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

//������Ϣ����
int32_t TCirQueueGen::Size()
{
    return _cur_count;
}

//�����Ƿ�Ϊ��
bool TCirQueueGen::Empty()
{
    if (_cur_count == 0)
        return true;
    return false;
}

//�����Ƿ�����
bool TCirQueueGen::Full()
{
    if (_cur_count == _max_count)
        return true;
    return false;
}

//�������,��cq���
bool TCirQueueGen::Clear()
{
    _head = 0;
    _tail = 0;
    _cur_count = 0;

    return true;
}
