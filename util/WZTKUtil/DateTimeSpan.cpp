#include "util/DateTimeSpan.h"
#include "util/StringEx.h"

DateTimeSpan::DateTimeSpan()
{
	_difftime = 0;
}

DateTimeSpan::DateTimeSpan( const DateTimeSpan& dtSpan )
{
	_difftime = dtSpan._difftime;
}

DateTimeSpan::DateTimeSpan( time_t t )
{
	_difftime = t;
}

DateTimeSpan::DateTimeSpan( int lDays,int nHours,int nMins,int nSecs )
{
	_difftime = nSecs + nMins*60 + nHours*60*60 + lDays*60*60*24;
}

int DateTimeSpan::GetDays() const
{
	return static_cast<int>(_difftime/(24*60*60));
}

int DateTimeSpan::GetHours() const
{
	return (_difftime%(24*60*60))/(60*60);
}

int DateTimeSpan::GetTotalHours() const
{
	return static_cast<int>(_difftime/(60*60));
}

int DateTimeSpan::GetMinutes() const
{
	return _difftime%(60*60)/60;
}

int DateTimeSpan::GetTotalMinutes() const
{
	return static_cast<int>(_difftime/60);
}

int DateTimeSpan::GetSeconds() const
{
	return _difftime%60;
}

int DateTimeSpan::GetTotalSeconds() const
{
	return static_cast<int>(_difftime);
}

time_t DateTimeSpan::GetTimeSpan() const
{
	return _difftime;
}

std::string DateTimeSpan::toString() const
{
	long lDays = GetDays();
	long lHours = GetHours();
	long lMins = GetMinutes();
	long lSecs = GetSeconds();
	return String::fromLong(lDays)+","+String::fromLong(lHours)+","+String::fromLong(lMins)+","+String::fromLong(lSecs);	
}

bool DateTimeSpan::operator==( DateTimeSpan span ) const
{
	return _difftime == span._difftime;
}

bool DateTimeSpan::operator!=( DateTimeSpan span ) const
{
	return !(_difftime == span._difftime);
}

bool DateTimeSpan::operator<( DateTimeSpan span ) const
{
	return _difftime < span._difftime;
}

bool DateTimeSpan::operator>( DateTimeSpan span ) const
{
	return _difftime > span._difftime;
}

bool DateTimeSpan::operator<=( DateTimeSpan span ) const
{
	return _difftime <= span._difftime;
}

bool DateTimeSpan::operator>=( DateTimeSpan span ) const
{
	return _difftime >= span._difftime;
}

DateTimeSpan DateTimeSpan::operator+( DateTimeSpan span ) const
{
	return DateTimeSpan(_difftime + span._difftime);
}

DateTimeSpan DateTimeSpan::operator-( DateTimeSpan span ) const
{
	return DateTimeSpan(_difftime - span._difftime);
}

DateTimeSpan& DateTimeSpan::operator+=( DateTimeSpan span )
{
	_difftime += span._difftime;
	return *this;
}

DateTimeSpan& DateTimeSpan::operator-=( DateTimeSpan span )
{
	_difftime -= span._difftime;
	return *this;
}