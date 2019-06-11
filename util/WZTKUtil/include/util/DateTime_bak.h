/**
* \copyright (C)2016 万兆天空
* \file DateTime.h 
* \brief 封装了一个日期时间DateTime
* \version 1.0
* \author Philippe Bernery
* \date 2016/11/11
*/

#ifndef OWDATETIME_H
#define OWDATETIME_H


#include <ctime>
#include <string>

#include "util/Date.h"
#include "util/TimeEx.h"
#include "util/DateTimeSpan.h"

/**
 * \brief 封装了一个日期时间DateTime
 * \ingroup DATE_TIME
 *
 * DateTime类的常用方法示例：
 * \code
 * // 以某个time_t时间构造对象
 * time_t t = time(NULL);
 * DateTime dtx(t);
 * // 以当前时间构造对象
 * DateTime dt;
 * // 转换为 YYYY-mm-DD HH:MM:SS格式的字符串
 * string DateTimeString = dt.toString();
 * // 从YYYY-mm-DD HH:MM:SS格式的字符串构造对象
 * DateTime dt1;
 * string DateTimeString1 = "1990-1-1 12:00:00";
 * bool isOk = DateTime::fromString(DateTimeString1,dt1);
 * // 加上或者减去某一段时间，得到一个时间。
 * DateTimeSpan span(1,1,1,1);
 * dt += span;
 * dt -= span;
 * // 转换为time_t
 * time_t t = dt.toTime_tFormat();
 * \endcode
 */
class DateTime
{
public:

	/**
	* Creates a date with the current time.
	*/
	DateTime();

	/**
	* Copy constructor.
	*/
	DateTime(const DateTime & datetime);

	/**
	* Creates a date from a day a month and a year.
	*
	* @param day day of the date
	* @param month month of the date
	* @param year year of the date
	*/
	DateTime(unsigned day, unsigned month, unsigned year,unsigned hour,unsigned minute,unsigned second);

	/**
	* Creates a datetime from time_t type value
	*/
	DateTime(std::time_t t);

	~DateTime();

	bool operator==(const DateTime & datetime) const;

	DateTime operator +( DateTimeSpan timeSpan ) const;
	DateTime operator -( DateTimeSpan timeSpan ) const;
	DateTimeSpan operator -( DateTime time ) const;
	DateTime& operator +=( DateTimeSpan span );
	DateTime& operator -=( DateTimeSpan span );
	DateTime& operator = (const DateTime& datetime){
		_day = datetime._day;
		_month = datetime._month;
		_year = datetime._year;
		_hour = datetime._hour;
		_minute = datetime._minute;
		_second = datetime._second;
		return *this;
	}
	/**
	* @return the day number of the date. Day range: 1-31.
	*/
	unsigned getDay() const;

	/**
	* Sets the date day.
	*
	* @see getDay()
	*/
	void setDay(unsigned day);

	/**
	* @return the month number of the date. Month range: 1-12.
	*/
	unsigned getMonth() const;

	/**
	* Sets the date month.
	*
	* @see getMonth()
	*/
	void setMonth(unsigned month);

	/**
	* Gets the date year. Year range: 0-infinite.
	*/
	unsigned getYear() const;

	/**
	* Sets date year.
	*
	* @see getYear()
	*/
	void setYear(unsigned year);
	/**
	* @return the hour of the time. range: 0-23.
	*/
	unsigned getHour() const;

	/**
	* Sets the hour.
	*/
	void setHour(unsigned hour);

	/**
	* @return the minute of the time. range: 0-59.
	*/
	unsigned getMinute() const;

	/**
	* Sets the minute.
	*/
	void setMinute(unsigned minute);

	/**
	* Gets the second. range: 0-59.
	*/
	unsigned getSecond() const;

	/**
	* Sets second.
	*/
	void setSecond(unsigned second);

	/**
	* Get Date part.
	*/
	Date getDate() const;

	/**
	* Get Time part.
	*/
	Time getTime() const;

	/**
	* @return a string representing the datetime. (e.g: "yyyy-mm-dd hh:MM:ss")
	*/
	std::string toString() const;

	/**
	* @return a DateTime representing the string. (e.g: "yyyy-mm-dd hh:MM:ss")
	*/
	static bool fromString(const std::string& strDateTime,DateTime& dt);

	/**
	 * @return a datetim representing the string as "yyyy/mm/dd/hh/mm/ss"
	 */
	static bool fromStringSlash(const std::string& strDateTime,DateTime& dt);

	/**
	* to time_t value;
	*/
	std::time_t toTime_tFormat() const;

	/**
	 * \brief 计算当前对象是星期几
	 * \return 返回0~6，分别代表周一到周日。
	 */
	int calcWeekDay() const;

	/**
	 * \brief 判断当前对象是否代表了一个正常的时间。用于构造函数使用年月日的情况，可能输入错误的值。
	 * \return 有效返回true,否则返回false.
	 */
	bool isValid() const;

	/**
	 * \brief 判断输入的时间是否是一个正常的时间。
	 * \return 有效返回true,否则返回false.
	 */
	static bool isValid(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);
	
	/**
	 * \brief 判断某个月有几天
	 * \return 闰年二月有变化，其他都是固定的值。
	 */
	static int daysOfMonth(int year, int month);

	/**
	 * \brief 判断某一年是否为闰年
	 * \return 是闰年返回true,否则返回false.
	 */
	static bool isLeapYear(int year);
private:
	unsigned _day;
	unsigned _month;
	unsigned _year;
	unsigned _hour;
	unsigned _minute;
	unsigned _second;
};
#endif