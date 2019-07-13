#include "util/DateTime.h"
#include "util/Logger.h"
#include "util/StringEx.h"

namespace qcutil
{
DateTime::DateTime() {
    std::time_t curTime = time(NULL);
    struct std::tm * timeinfo = std::localtime(&curTime);

    setDay(timeinfo->tm_mday);
    setMonth(timeinfo->tm_mon + 1);
    setYear(timeinfo->tm_year + 1900);

    setHour(timeinfo->tm_hour);
    setMinute(timeinfo->tm_min);
    setSecond(timeinfo->tm_sec);
}

DateTime::DateTime(const DateTime & datetime) {
    setDay(datetime._day);
    setMonth(datetime._month);
    setYear(datetime._year);
    setHour(datetime._hour);
    setMinute(datetime._minute);
    setSecond(datetime._second);
}

DateTime::DateTime(std::time_t t) {
    struct std::tm * timeinfo = std::localtime(&t);

    setDay(timeinfo->tm_mday);
    setMonth(timeinfo->tm_mon + 1);
    setYear(timeinfo->tm_year + 1900);

    setHour(timeinfo->tm_hour);
    setMinute(timeinfo->tm_min);
    setSecond(timeinfo->tm_sec);
}

DateTime::DateTime(unsigned day, unsigned month, unsigned year, unsigned hour, unsigned minute, unsigned second)
: _day(day),
_month(month),
_year(year),
_hour(hour),
_minute(minute),
_second(second) {
}

DateTime::~DateTime() {

}

bool DateTime::operator==(const DateTime & datetime) const {
    return ((_day == datetime._day)
            && (_month == datetime._month)
            && (_year == datetime._year)
            && (_hour == datetime._hour)
            && (_minute == datetime._minute)
            && (_second == datetime._second));
}

bool DateTime::operator!=(const DateTime& datetime) const {
    return !(*this == datetime);
}

unsigned DateTime::getDay() const {
    return _day;
}

void DateTime::setDay(unsigned day) {
    if (day < 1) {
        LOG_FATAL("day cannot be < 1");
    } else if (day > 31) {
        LOG_FATAL("day cannot be > 31");
    }

    _day = day;
}

unsigned DateTime::getMonth() const {
    return _month;
}

void DateTime::setMonth(unsigned month) {
    if (month < 1) {
        LOG_FATAL("month cannot be < 1");
    } else if (month > 12) {
        LOG_FATAL("month cannot be > 12");
    }

    _month = month;
}

unsigned DateTime::getYear() const {
    return _year;
}

void DateTime::setYear(unsigned year) {
    _year = year;
}

unsigned DateTime::getHour() const {
    return _hour;
}

void DateTime::setHour(unsigned hour) {
    if (hour > 23) {
        LOG_FATAL("hour cannot be > 23");
    }

    _hour = hour;
}

unsigned DateTime::getMinute() const {
    return _minute;
}

void DateTime::setMinute(unsigned minute) {
    if (minute > 59) {
        LOG_FATAL("minute cannot be > 59");
    }

    _minute = minute;
}

unsigned DateTime::getSecond() const {
    return _second;
}

void DateTime::setSecond(unsigned second) {
    if (second > 59) {
        LOG_FATAL("second cannot be > 59");
    }

    _second = second;
}

std::string DateTime::toString() const {
    std::string year = String::fromNumber(_year);
    std::string month = String::fromNumber(_month);
    std::string day = String::fromNumber(_day);

    std::string hour = String::fromNumber(_hour);
    std::string minute = String::fromNumber(_minute);
    std::string second = String::fromNumber(_second);

    if (month.size() == 1) {
        month = "0" + month;
    }

    if (day.size() == 1) {
        day = "0" + day;
    }

    if (hour.size() == 1) {
        hour = "0" + hour;
    }

    if (minute.size() == 1) {
        minute = "0" + minute;
    }

    if (second.size() == 1) {
        second = "0" + second;
    }

    return year + "-" + month + "-" + day + " " + hour + ":" + minute + ":" + second;
}

Date DateTime::getDate() const {
    return Date(_day, _month, _year);
}

Time DateTime::getTime() const {
    return Time(_hour, _minute, _second);
}

std::time_t DateTime::toTime_tFormat() const {
    //time_t mktime(struct tm * timeptr); 
    std::tm time_info;
    time_info.tm_year = _year - 1900;
    time_info.tm_mon = _month - 1;
    time_info.tm_mday = _day;
    time_info.tm_hour = _hour;
    time_info.tm_min = _minute;
    time_info.tm_sec = _second;
    time_info.tm_isdst = 0;
    return mktime(&time_info);
}

DateTime DateTime::operator+(DateTimeSpan timeSpan) const {
    std::time_t t = toTime_tFormat();
    std::time_t t_span = timeSpan.GetTotalSeconds();
    return DateTime(t + t_span);
}

DateTime DateTime::operator-(DateTimeSpan timeSpan) const {
    std::time_t t = toTime_tFormat();
    std::time_t t_span = timeSpan.GetTotalSeconds();
    return DateTime(t - t_span);
}

DateTimeSpan DateTime::operator-(DateTime time) const {
    std::time_t my_t = toTime_tFormat();
    std::time_t op_t = time.toTime_tFormat();
    return DateTimeSpan(my_t - op_t);
}

DateTime& DateTime::operator+=(DateTimeSpan span) {
    std::time_t t = toTime_tFormat();
    std::time_t t_span = span.GetTotalSeconds();
    *this = DateTime(t + t_span);
    return *this;
}

DateTime& DateTime::operator-=(DateTimeSpan span) {
    std::time_t t = toTime_tFormat();
    std::time_t t_span = span.GetTotalSeconds();
    *this = DateTime(t - t_span);
    return *this;
}

bool DateTime::fromString(const std::string& strDateTime, DateTime& dt) {
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;

    int iret = sscanf(strDateTime.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    if (iret != 6) {
        return false;
    } else {
        dt = DateTime(day, month, year, hour, minute, second);
        return true;
    }
}

bool DateTime::fromStringSlash(const std::string& strDateTime, DateTime& dt) {
    int year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0;
    int iret = sscanf(strDateTime.c_str(), "%d/%d/%d/%d/%d/%d", &year, &month, &day, &hour, &minute, &second);
    if (iret != 6) {
        return false;
    } else {
        dt = DateTime(day, month, year, hour, minute, second);
        return true;
    }
}

int DateTime::calcWeekDay() const {
    time_t tMyDateTime = toTime_tFormat();
    if (tMyDateTime < 0) {
        return -1;
    }
    struct tm* tmpMyDate;
    tmpMyDate = localtime(&tMyDateTime);
    return tmpMyDate->tm_wday;
}

#ifndef ASSERT
#define ASSERT
#endif

int DateTime::daysOfMonth(int year, int month) {
    ASSERT(month >= 1 && month <= 12);

    static int daysOfMonthTable[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (month == 2 && isLeapYear(year))
        return 29;
    else
        return daysOfMonthTable[month];
}

bool DateTime::isValid(int year,
        int month,
        int day,
        int hour,
        int minute,
        int second,
        int millisecond,
        int microsecond) {
    return
    (year >= 0 && year <= 9999) &&
            (month >= 1 && month <= 12) &&
            (day >= 1 && day <= daysOfMonth(year, month)) &&
            (hour >= 0 && hour <= 23) &&
            (minute >= 0 && minute <= 59) &&
            (second >= 0 && second <= 59) &&
            (millisecond >= 0 && millisecond <= 999) &&
            (microsecond >= 0 && microsecond <= 999);
}

bool DateTime::isValid() const {
    return
    (_year >= 0 && _year <= 9999) &&
            (_month >= 1 && _month <= 12) &&
            (_day >= 1 && _day <= daysOfMonth(_year, _month)) &&
            (_hour >= 0 && _hour <= 23) &&
            (_minute >= 0 && _minute <= 59) &&
            (_second >= 0 && _second <= 59);
}

bool DateTime::isLeapYear(int year) {
    return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
}
}
