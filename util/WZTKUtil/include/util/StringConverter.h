/**
 * \copyright (C)2016 万兆天空
 * \file StringConverter.h
 * \brief 声明utf8编码和系统默认字符串编码的转换（简体中文系统对应GB2312）
 * 
 * - v1.0
 *   -# LocalStringToUtf8 系统默认字符串转换为utf-8。
 *   -# UTF8ToLocalString utf-8字符串转换为系统默认。
 * - v1.1
 *   -# UTF8ToUnicode utf-8转换为UNICODE
 *   -# UnicodeToUTF8 Unicode转换为UTF-8
 *
 * \author FDD
 * \date 2017/8/17
 * \version 1.1
 */
#pragma once

#include <string>
using std::string;
using std::wstring;

/**
 * \brief 字符串转换类
 * \ingroup  STRINGS
 */
class StringConverter
{
public:
	/**
	 * \brief 系统默认转换为utf-8
	 */
	static string LocalStringToUtf8( const string& localString );
	/**
	 * \brief utf-8转换为系统默认
	 */
	static string UTF8ToLocalString( const string& utfString );
	/**
	 * \brief utf-8转换为UNICODE
	 */
	static std::wstring UTF8ToUnicode( const string& utfString );
	/**
	 * \brief UNICODE转换为utf-8
	 */
	static std::string UnicodeToUTF8(const wstring& str);
protected:
private:
};