#include "pch.h"
#include "util_time.h"
#include "util_string.h"
#include <sys/timeb.h>
#include "log_mgr.h"
#include "util_strop.h"

#define DaySeconds 86400
#define WeekSeconds 604800

const char* szfmt[VST_END] = {
	"%Y-%m-%d %H:%M:%S",
	"%Y-%m-%d",
	"%H:%M:%S",
	"%Y%m%d%H%M%S",
	"%Y%m%d",
	"%H%M%S"
};

time_t tmCurr = std::time(nullptr);
extern time_t GetCurrTime()
{ 
#ifdef _WIN32
	return std::time(nullptr);
#endif
	return tmCurr;
}

extern time_t SetCurrTime()
{
	tmCurr = std::time(nullptr);
	return tmCurr;
}

time_t GetMicroSec()
{
	auto tm_now = std::chrono::system_clock::now();
	auto duration_in_ms = std::chrono::duration_cast<std::chrono::microseconds>(tm_now.time_since_epoch());
	return duration_in_ms.count();
}
time_t GetMillSec()
{
	auto tm_now = std::chrono::system_clock::now();
	auto duration_in_ms = std::chrono::duration_cast<std::chrono::milliseconds>(tm_now.time_since_epoch());
	return duration_in_ms.count();
}
time_t GetNanoSec()
{
	auto tm_now = std::chrono::system_clock::now();
	auto duration_in_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(tm_now.time_since_epoch());
	return duration_in_ms.count();
}

extern short GetTimeZone()
{
	short cTimeZone = GetLocalZoneHour();
	return -cTimeZone;
}

time_t GetGmTime(time_t nTime)
{
	struct tm struct_tm;
	gmtime_s(&struct_tm, &nTime);
	return mktime(&struct_tm);
}

time_t GetZeroTime(time_t nTime)
{
	tm tmZero;
	localtime_s(&tmZero, &nTime);
	tmZero.tm_hour = 0;
	tmZero.tm_min = 0;
	tmZero.tm_sec = 0;
	return mktime(&tmZero);
}

time_t GetZeroMonth(time_t nTime)
{
	tm tmMon;
	localtime_s(&tmMon, &nTime);
	tmMon.tm_mday = 1;
	tmMon.tm_hour = 0;
	tmMon.tm_min = 0;
	tmMon.tm_sec = 0;
	return mktime(&tmMon);
}

time_t GetZeroNextMonth(time_t nTime)
{
	struct tm tmMon;
	localtime_s(&tmMon, &nTime);
	if (tmMon.tm_mon >= 11)
	{
		tmMon.tm_year += 1;
		tmMon.tm_mon = 0;
	}
	else
		tmMon.tm_mon += 1;

	tmMon.tm_mday = 1;
	tmMon.tm_hour = 0;
	tmMon.tm_min = 0;
	tmMon.tm_sec = 0;
	return mktime(&tmMon);
}

short GetLocalZoneMin()
{
	static short shZone = 0;
	if (shZone) return shZone;
	struct timeb tp;
	ftime(&tp);
	shZone = tp.timezone;
	return shZone;
}

short GetLocalZoneHour()
{
	static short shZone = 0;
	if (shZone) return shZone;
	shZone = GetLocalZoneMin() / 60;
	return shZone;
}

short GetLocalZoneSec()
{
	static short shZone = 0;
	if (shZone) return shZone;
	shZone = GetLocalZoneMin() * 60;
	return shZone;
}

std::string CurrTimeToString(time_str_type etype, time_t tmNow)
{
	std::time_t tmvalue = tmNow;
	if (tmvalue == 0)
		tmvalue = GetCurrTime();
	std::tm tmsturct;
	localtime_s(&tmsturct, &tmvalue);
	if (etype == VST_DAYSECS)
		return std::to_string(tmsturct.tm_hour*3600 + tmsturct.tm_min*60 + tmsturct.tm_sec);

	char ch[20];
	std::strftime(ch, 20, szfmt[etype], &tmsturct);
	return ch;
}

int64_t CurrTimeToNumber(time_str_type etype, time_t tmNow)
{
	std::string strs = CurrTimeToString(etype, tmNow);
	if (strs.empty())
		return 0;
	try
	{
		return std::stoll(strs, nullptr, 10);
	}
	catch (std::exception& e)
	{
		Log_Error("%s-%s", strs.c_str(), e.what());
		return 0;
	}
}

int GetLocalDays(time_t llGmTime, int nClockSeconds/* = 0*/)
{
	int64 ret = (llGmTime - GetLocalZoneSec() - nClockSeconds) / DaySeconds;
	return (int)ret;
}

bool IsSameDay(time_t llTime1, time_t llTime2, int nClockSeconds/* = 0*/)
{
	return (GetLocalDays(llTime1, nClockSeconds) == GetLocalDays(llTime2, nClockSeconds));
}

int GetLocalWeeks(time_t llGmTime, int nClockSeconds/* = 0*/)
{
	int64 ret = (llGmTime - GetLocalZoneSec() - 4 * DaySeconds - nClockSeconds) / WeekSeconds;
	return (int)ret;
}

bool IsSameWeek(time_t llTime1, time_t llTime2, int nClockSeconds /* = 0*/)
{
	return (GetLocalWeeks(llTime1, nClockSeconds) == GetLocalWeeks(llTime2, nClockSeconds));
}

bool DateToTime(time_t& tRet, const std::string& strDate)
{
	tRet = 0;
	bool bRet = true;
	struct  std::tm tm1;
	std::vector<std::string> vecDate;
	str_split(strDate, ' ', vecDate);
	if (vecDate.size() < 2)
	{
		Log_Error("error! date:%s", strDate.c_str());
		return false;
	}
	else
	{
		std::vector<int> veciDate, veciTime;
		str_split_num(vecDate[0], '-', veciDate);
		str_split_num(vecDate[1], ':', veciTime);
		if (veciDate.size() < 3 || veciTime.size() < 3)
		{
			Log_Error("error! date:%s", strDate.c_str());
			tm1.tm_year = 0;
			tm1.tm_mon = 0;
			tm1.tm_hour = 0;
			tm1.tm_isdst = 0;
		}
		else
		{
			tm1.tm_year = veciDate[0] - 1900;
			tm1.tm_mon = veciDate[1] - 1;
			tm1.tm_mday = veciDate[2];

			tm1.tm_hour = veciTime[0];
			tm1.tm_min = veciTime[1];
			tm1.tm_sec = veciTime[2];
		}
	}
	tRet = mktime(&tm1);
	return bRet;
}

