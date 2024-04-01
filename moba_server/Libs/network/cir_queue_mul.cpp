#include "pch.h"
#include "cir_queue_mul.h"

//��ʼ����Ϣ����
bool TCirQueueMul::Init(uint32_t max_count)
{
    std::unique_lock<std::mutex> lck(_mutex);

    if (max_count == 0)
        return false;
    //��ʼ������
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

//�ͷ��ڴ�
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

//����Ϣpush_back��������(���߳�push_back����Ҫ����)
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
//��push������Ϣ������pop������
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

//����Ϣ�Ӷ�����pop_from(һ���߳�pop,���Բ�Ҫ����)
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

//push������Ϣ����
int32_t TCirQueueMul::Size()
{
    return _cur_count;
}
//push�����Ƿ�Ϊ��
bool TCirQueueMul::Empty()
{
    return _cur_count == 0;
}
//push�����Ƿ�����
bool TCirQueueMul::Full()
{
    return _cur_count == _max_count;
}

//������ˣ���cq���
bool TCirQueueMul::Clear()
{
    _tail = 0;
    _cur_count = 0;

    return true;
}
