/**
 * \copyright (C)2016 万兆天空
 * \file OSVersion.h 
 * \brief 定义获取Windows版本号、判断是不是win8或者win8.1的接口。
 * 
 * v1.0
 * - GetWinMajorMinorVersion 获取数字形式的版本号。
 * - GetWindowsVersion 获取Windows版本信息。
 * - IsWindows8Or8Point1 判断是否是win8或者win8.1。
 * - isWow64 判断本进程是否是wow64进程。
 * - IsOsWindowsVistaorLater 判断操作系统是否是vista以及之后的版本。
 *
 * \author fanduoduo
 * \date 2016/11/10
 * \version 1.0
 */

#pragma once

#include <windows.h>
#include <string>
using std::string;

/**
 * \defgroup WIN_VER 获取windows版本信息
 * \brief 获取windows版本信息
 * \{
 */

/**
 * \brief 获取数字形式的版本号
 * \param [out] major 高版本号
 * \param [out] minor 低版本号
 * \return 成功返回true否则返回false.
 */
bool GetWinMajorMinorVersion(DWORD& major, DWORD& minor);


/**
 * \brief 获取Windows版本信息
 * \param[out][opt] pOsVer 系统版本信息,如不需要这个信息可以传入NULL。
 * \param[out][opt] pSysInfo 系统信息，如果不需要这个信息可以传入NULL。
 * \return 返回字符串格式的版本信息，并携带SP信息，例如windows 7 sp1。
 * \remark 
 * 函数返回值为：%WinOSVer% Service Pack x.例如Windows 10 Server Service Pack 3.
 * 其中WinOSVer包括如下：
 *   - Windows 10 Server 
 *   - Windows 10
 *   - Windows Server 2012 R2
 *   - Windows 8.1
 *   - Windows Server 2012
 *   - Windows 8
 *   - Windows Server 2008 R2
 *   - Windows 7
 *   - Windows Server 2008
 *   - Windows Vista
 *   - Windows XP x64
 *   - Windows Server 2003
 *   - Windows XP
 *   - Windows 2000
 * 如果没有SP则没有后面的Service Pack x.
 */
std::string GetWindowsVersion(OSVERSIONINFOEX* pOsVer,SYSTEM_INFO* pSysInfo);


/**
 * \brief 判断是否是win8或者win8.1
 * \return 如果是win8或者win8.1返回TRUE,否则返回FALSE.
 */
BOOL IsWindows8Or8Point1();


/**
 * \brief 判断本进程是否是wow64进程
 * 
 * \return 如果本进程是WOW64进程返回TRUE,否则返回FALSE.
 * \note 仅当本进程是32位进程时此例程的返回值才能用于判断是否是64位系统。
 */
BOOL isWow64();


/**
 * \brief 通过获取OS版本号是否 > 5判断操作系统是否是vista以及之后的版本。
 * 
 * \return : 如果是返回TRUE，否则返回FALSE。
 */
BOOL IsOsWindowsVistaorLater();

/**
 * \}
 */
