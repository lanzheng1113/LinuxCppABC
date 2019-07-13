/**
 * \copyright (C)2016 万兆天空
 * \file StringConverter.cpp 
 * \brief 实现utf8编码和系统默认字符串编码的转换（简体中文系统对应GB2312）. \see StringConverter.h 
 * \author FDD
 * \date 2017/8/15
 * \version 1.0
 */
#include <Windows.h>
#include "util/StringConverter.h"

string StringConverter::LocalStringToUtf8( const string& str )
{
	int nwLen = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	wchar_t * pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, (nwLen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
	int nLen = WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char * pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);
	WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
	std::string retStr(pBuf);
	delete[]pwBuf;
	delete[]pBuf;
	pwBuf = NULL;
	pBuf = NULL;
	return retStr;
}

string StringConverter::UTF8ToLocalString( const string& str )
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	wchar_t * pwBuf = new wchar_t[nwLen + 1];
	memset(pwBuf, 0, (nwLen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);
	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
	std::string retStr = pBuf;
	delete[]pBuf;
	delete[]pwBuf;
	pBuf = NULL;
	pwBuf = NULL;
	return retStr;
}

std::wstring StringConverter::UTF8ToUnicode( const string& utfString )
{
	int nwLen;
	wchar_t* pwBuf;
	std::wstring wszUnicode;
	wchar_t wszBuffer[512+1];
	if (utfString.empty())
	{
		return wszUnicode;
	}
	nwLen = MultiByteToWideChar(CP_UTF8, 0, utfString.c_str(), -1, NULL, 0); 
	if (nwLen <= 0){
		return wszUnicode;
	}
	if (nwLen <= 512)
	{
		pwBuf = wszBuffer;
	}
	else
	{
		pwBuf = new wchar_t[nwLen + 1];
	}
	if (pwBuf)
	{
		memset(pwBuf, 0, (nwLen  + 1)* 2); 
		MultiByteToWideChar(CP_UTF8, 0, utfString.c_str(), -1, pwBuf, nwLen); 
		wszUnicode = pwBuf;
	}
	if (pwBuf != wszBuffer)
	{
		delete[] pwBuf;
	}
	pwBuf = NULL; 
	return wszUnicode;
}

std::string StringConverter::UnicodeToUTF8( const wstring& str )
{
	std::string wszANSI;
	char buffer[512+1];
	char* pBuf;
	int nLen;

	if (str.empty())
	{
		return wszANSI;
	}
	nLen = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL); 
	if (nLen <= 0){
		return wszANSI;
	}

	if (nLen <= 512)
	{
		pBuf = buffer;
	}
	else
	{
		pBuf = new char[nLen + 1];
	}

	if (pBuf)
	{
		memset(pBuf, 0, (nLen  + 1)); 
		WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, pBuf, nLen, NULL, NULL); 
		wszANSI = pBuf;
	}

	if (pBuf != buffer)
	{
		delete[] pBuf;
	}
	pBuf = NULL; 

	return wszANSI;
}
