#include "stdafx.h"
#include "Clock.h"
#include "JPDefine.h"

Clock::Clock()
{
	serverStartTick_ = this->systemTick();
}

Clock::~Clock()
{
}

std::time_t Clock::strToTick(std::wstring str, const WCHAR *fmt)
{
	int	year = 0;
	int	month = 0;
	int	day = 0;
	int	hour = 0;
	int	minute = 0;
	int	second = 0;

	swscanf_s(str.c_str(), fmt, &year, &month, &day, &hour, &minute, &second);

	//		   초,		분,    시,  일,  월(0부터시작), 년(1900년부터시작)
	tm time = { second, minute, hour, day, month - 1, year - 1900 };

	return mktime(&time);
}

std::time_t Clock::serverStartTick()
{
	return serverStartTick_;
}

std::time_t Clock::systemTick()
{
	return system_clock::to_time_t(system_clock::now());
}

std::wstring Clock::tickToStr(std::time_t tick, const WCHAR *fmt)
{
	array<WCHAR, SIZE_128> timeStr;

	tm time;
	localtime_s(&time, &tick);
	wcsftime(timeStr.data(), timeStr.size(), fmt, &time);

	return timeStr.data();
}

std::wstring Clock::nowTime(const WCHAR *fmt)
{
	return this->tickToStr(this->systemTick(), fmt);
}

std::wstring Clock::nowTimeWithMilliSec(const WCHAR *fmt)
{
#if 0
	timePoint now = system_clock::now();
	timePoint oldSecond = system_clock::from_time_t(this->systemTick());
	duration<double> milliSecond = now - oldSecond;
	array<WCHAR, SIZE_8> milliStr;
	snwprintf(milliStr, L"%03d", (int)(milliSecond.count() * 1000));
#else
	high_resolution_clock::time_point point = high_resolution_clock::now();
	milliseconds ms = duration_cast<milliseconds>(point.time_since_epoch());

	seconds s = duration_cast<seconds>(ms);
	std::time_t t = s.count();
	std::size_t fractionalSeconds = ms.count() % 1000;
	array<WCHAR, SIZE_8> milliStr;
	snwprintf(milliStr, L"%03d", (int)(fractionalSeconds));
#endif
	std::wstring timeString = this->tickToStr(this->systemTick(), fmt);
	timeString += L".";
	timeString += milliStr.data();
	return timeString;
}

std::wstring Clock::today()
{
	return this->tickToStr(this->systemTick(), DATE_FORMAT);
}

std::wstring Clock::tomorrow()
{
	return this->tickToStr(this->systemTick() + DAY_TO_TICK(1), DATE_FORMAT);
}

std::wstring Clock::yesterday()
{
	return this->tickToStr(this->systemTick() - DAY_TO_TICK(1), DATE_FORMAT);
}

DayOfTheWeek Clock::todayOfTheWeek()
{
	tm time;
	std::time_t tick = this->systemTick();
	localtime_s(&time, &tick);
	return (DayOfTheWeek)time.tm_wday;
}