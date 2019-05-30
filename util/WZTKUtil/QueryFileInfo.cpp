#include <Windows.h>
#include "util/CommonWinFuns.h"
#include <string>

using std::string;

std::string GetFileLegalCopyright(CHAR * strFilePath)
{
	DWORD dwSize;
	DWORD dwRtn;
	string szVersion;
	//获取版本信息大小
	dwSize = GetFileVersionInfoSizeA(string(strFilePath).c_str(), NULL);
	if (dwSize == 0)
	{
		return "";
	}
	CHAR *pBuf;
	pBuf = new CHAR[dwSize + 1];
	memset(pBuf, 0, dwSize + 1);
	//获取版本信息
	dwRtn = GetFileVersionInfoA(string(strFilePath).c_str(), NULL, dwSize, pBuf);
	if (dwRtn == 0)
	{
		delete [] pBuf;
		return "";
	}
	LPVOID lpBuffer = NULL;
	UINT uLen = 0;
	//版本资源中获取信息
	dwRtn = VerQueryValueA(pBuf,
		"\\StringFileInfo\\080404b0\\LegalCopyright",
		//0804中文
		//04b0即1252,ANSI
		//可以从ResourceView中的Version中BlockHeader中看到
		//可以测试的属性
		/*
		CompanyName
		FileDescription
		FileVersion
		InternalName
		LegalCopyright
		OriginalFilename
		ProductName
		ProductVersion
		Comments
		LegalTrademarks
		PrivateBuild
		SpecialBuild
		*/
		&lpBuffer,
		&uLen);
	if (dwRtn == 0)
	{
		delete [] pBuf;
		return "";
	}
	szVersion = (CHAR*)lpBuffer;
	delete [] pBuf;
	return szVersion;
}


std::string GetPEFileVersionInfo(const std::string& strFilePath, const std::string& strVersionKey)
{
	DWORD dwSize;
	DWORD dwRtn;
	string szVersion;
	//获取版本信息大小
	dwSize = GetFileVersionInfoSizeA(strFilePath.c_str(), NULL);
	if (dwSize == 0)
	{
		return "";
	}
	CHAR *pBuf;
	pBuf = new CHAR[dwSize + 1];
	memset(pBuf, 0, dwSize + 1);
	//获取版本信息
	dwRtn = GetFileVersionInfoA(strFilePath.c_str(), NULL, dwSize, pBuf);
	if (dwRtn == 0)
	{
		delete []pBuf;
		return "";
	}
	LPVOID lpBuffer = NULL;
	UINT uLen = 0;
	//版本资源中获取信息
	string versionKeyZh = "\\StringFileInfo\\080404b0\\";
	versionKeyZh += strVersionKey;
	string versionKeyEn = "\\StringFileInfo\\040904b0\\";		
	versionKeyEn += strVersionKey;
	string versionKeyEn2 = "\\StringFileInfo\\040904e4\\";
	versionKeyEn2 += strVersionKey;
	//中文资源
	dwRtn = VerQueryValueA(pBuf,
		versionKeyZh.c_str(),
		&lpBuffer,
		&uLen);
	//英文资源
	if (dwRtn == 0)
	{
		dwRtn = VerQueryValueA(pBuf,
			versionKeyEn.c_str(),
			&lpBuffer,
			&uLen);
	}
	//英文资源2
	if (dwRtn == 0)
	{
		dwRtn = VerQueryValueA(pBuf,
			versionKeyEn2.c_str(),
			&lpBuffer,
			&uLen);
	}
	//失败了
	if (dwRtn == 0)
	{
		delete []pBuf;
		return "";
	}		
	szVersion = (CHAR*)lpBuffer;
	delete [] pBuf;
	return szVersion;
}
