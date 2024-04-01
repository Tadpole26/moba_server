#pragma once
#include <assert.h>
#include <string>
#include <ctime>
#include "dtype.h"

//ʱ�䴦����

enum time_str_type
{
	VST_ALL,				//2010-01-01 01:00:00
	VST_DATE,				//2010-01-01
	VST_TIME,				//01:01:01
	VST_ALLSIMPLE,			//20100101010101
	VST_DATESIMPLE,			//20100101
	VST_TIMESIMPLE,			//010101
	VST_DAYSECS,			//3600*24
	VST_END,
};


//��õ��ص�ϵͳʱ��
extern time_t SetCurrTime();
extern time_t GetCurrTime();
extern time_t GetMicroSec();
extern time_t GetMillSec();
extern time_t GetNanoSec();
//���ʱ��
extern short GetTimeZone();
//��ǰʱ��ת�����ַ���
std::string CurrTimeToString(time_str_type etype, time_t tmNow = 0);
//��ǰʱ��ת��������
int64_t CurrTimeToNumber(time_str_type etype, time_t tmNow = 0);
//��õ�ǰ����ʱ��,��������ʱ��(0ʱ��)
extern time_t GetGmTime(time_t nTime = GetCurrTime());
//���0��ʱ���
extern time_t GetZeroTime(time_t nTime = GetCurrTime());
//����µĵ�һ�쿪ʼ
extern time_t GetZeroMonth(time_t nTime = GetCurrTime());
//����¸��µ�һ�쿪ʼ
extern time_t GetZeroNextMonth(time_t nTime = GetCurrTime());
//��õ�ǰʱ��͸�������ʱ���ʱ���(����)
extern short GetLocalZoneMin();
//��õ�ǰʱ��͸�������ʱ���ʱ���(Сʱ)
extern short GetLocalZoneHour();
//��õ�ǰʱ��͸�������ʱ���ʱ���(��)
extern short GetLocalZoneSec();
int GetLocalDays(time_t llGmTime, int nClockSeconds = 0);
bool IsSameDay(time_t llTime1, time_t llTime2, int nClockSeconds = 0);
int GetLocalWeeks(time_t llGmTime, int nClockSeconds = 0);
bool IsSameWeek(time_t llTime1, time_t llTime2, int nClockSeconds  = 0);

extern bool DateToTime(time_t& tRet, const std::string& strDate);

//��ʱ�ṹ
class IntervalTime
{
public:
	IntervalTime() : m_llBeginTime(GetMicroSec()){ } 
	~IntervalTime() { }

	void Begin() { m_llBeginTime = GetMicroSec(); }
	int64 Interval() 
	{ 
		int64 llCurTime = GetMicroSec();
		int64 llInterTime = llCurTime - m_llBeginTime;
		m_llBeginTime = llCurTime;
		return llInterTime;
	}
protected:
	int64 m_llBeginTime = 0;
};

class BeginEndTime
{
public:
	BeginEndTime()
	{
		m_llStartTime = GetMicroSec();
	}

	~BeginEndTime() { }

	int64 StartTime()
	{
		m_llStartTime = GetMicroSec();
		m_llEndTime = 0;
		return m_llStartTime;
	}

	int64 Ms()
	{
		if (m_llEndTime == 0)
			m_llEndTime = GetMicroSec();
		return (m_llEndTime - m_llStartTime);
	}

	void ResetEnd()
	{
		m_llEndTime = 0;
	}

	void Clear()
	{
		m_llStartTime = 0;
		m_llEndTime = 0;
	}
private:
	time_t m_llStartTime = 0;
	time_t m_llEndTime = 0;
};


class GenTimer
{
public:
	GenTimer() : _value(0), _interval(0)
	{
	}

	virtual ~GenTimer() 
	{
		_value = 0;
		_interval = 0;
	}

	int64 Interval() { return _interval; }
	void Init(int64 interval) 
	{
		_interval = interval;
		_value = 0;
	}
	void ResetOn() { _value = 0; }
	int64 now() { return _value; }
	bool On(int64 add = 1)
	{
		_value += add;
		if (_value >= _interval)
		{
			_value = 0;
			return true;
		}
		return false;
	}

protected:
	int64 _value = 0;
	int64 _interval = 0;
};

template <int64 TICKS>
class NumTimer : public GenTimer
{
public:
	NumTimer()
	{
		_value = 0;
		_interval = TICKS;
	}
	virtual ~NumTimer()
	{
		_value = 0;
		_interval = TICKS;
	}
};

template <time_t TICKSEC>
class SecTimer
{
public:
	SecTimer()
	{
		_tmBegin = 0;
		_interval = TICKSEC;
	}
	SecTimer(time_t tmBegin)
	{
		_tmBegin = tmBegin;
		_interval = TICKSEC;
	}
	virtual ~SecTimer()
	{
		_tmBegin = 0;
	}

	void Init(time_t tmBegin, time_t interval)
	{
		_tmBegin = tmBegin;
		_interval = interval;
	}

	void ResetOn(time_t tmNow)
	{
		_tmBegin = tmNow;
	}

	bool On(time_t tmNow)
	{
		if (_tmBegin == 0)
			_tmBegin = tmNow;

		if (tmNow >= (time_t)(_tmBegin + _interval))
		{
			_tmBegin = tmNow;
			return true;
		}
		return false;
	}
public:
	time_t _tmBegin = 0;
	time_t _interval = 0;
};