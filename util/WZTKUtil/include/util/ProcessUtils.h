/**
 * \copyright (C)2016 万兆天空
 * \file ProcessUtils.h
 * \brief 声明一些和读取进程信息有关的函数
 *
 * v1.0
 * - IsProcessExists 判断指定PID的进程是否存在。
 * - GetPathFromProcessId 获取指定PID的进程的路径。
 * - GetProcessIdFromPath 获取第一个匹配指定进程路径的进程ID 
 * - DosDevicePath2LogicalPath 将DOS设备路径（\\Device\\HarddiskVolume1\x86.sys）转换为逻辑路径（C:\x86.sys）
 *
 * \author fanduoduo
 * \date 2016/11/10
 * \version 1.0
 */

#pragma once
#include <windows.h>
#include <string>

#ifndef OUT
#define OUT
#endif

/**
 * \defgroup PROCE_INFO 获取进程信息
 * \{
 */

namespace Process
{
	/**
 * \brief 判断指定PID的进程是否存在
 * \return 存在返回TRUE，否则返回FALSE
 */
BOOL IsProcessExists(DWORD pid);


/**
 * \brief 获取指定PID的进程的路径。
 * \return 找到则返回指定进程的路径，否则返回空字符串
 */
std::string GetPathFromProcessId(DWORD pid);


/**
 * \brief 获取第一个匹配指定进程路径的进程ID
 * \return 找到则返回PID，否则返回0
 */
DWORD GetProcessIdFromPath(const std::string& strFilePath);


/**
 * \brief 根据进程名获取进程ID
 * \return 返回进程ID，如果没有找到返回0
 */
DWORD GetProcessIdByName(const std::string& strProcessName);

/**
 * \brief 查找一个包含给定名字的进程ID
 * 
 * \param strProcessName 希望进程名中包含的字符串。
 * \param processName 找到的进程名，如果没有找到，这个值将被赋予空字符串。
 * \return 找到的进程ID，如果没有找到则返回0
 */
DWORD GetProcessIdLikeName(const std::string& strProcessName, OUT std::string& processName);

/**
 * \brief 杀死所有对应名字的进程
 * \return 返回被杀死进程的个数
 */
DWORD KillProcess(const std::string& strProcessName);

/**
 * \brief 将DOS设备路径（\\Device\\HarddiskVolume1\x86.sys）转换为逻辑路径（C:\x86.sys）
 * \return 如转换失败则返回空字符串。
 */
std::string DosDevicePath2LogicalPath(const wchar_t* lpszDosPath);	


/**
 * \brief 根据进程名字打开进程。
 */
HANDLE OpenProcessByPName(const std::string& strProcessName);

/**
 * \brief 提升自身进程权限为Debug
 * \param TRUE enable,FALSE disable.
 * \return 提权失败返回false，调用GetlastError获取详细信息,提权成功返回true.
 */
BOOL SetSeDebugPrivilege(BOOL bEnablePrivilege);

/**
 * \brief 暂停指定ID的进程
 * 
 * \param ProcessId 需要暂停进程的ID
 * \return 返回一个NTSTATUS状态值，需要使用NT_SUCCESS宏判断是否成功。
 */
NTSTATUS SuspendProcessById(DWORD ProcessId);

/**
 * \brief 恢复指定ID的进程
 * 
 * \param ProcessId 需要恢复运行进程的ID
 * \return 返回一个NTSTATUS状态值，需要使用NT_SUCCESS宏判断是否成功。
 */
NTSTATUS ResumeProcessById(DWORD ProcessId);

/**
 * \brief 暂停一个进程运行，你必须先获取这个进程的句柄以传递给这个函数。
 * 
 * \param hProcess 需要暂停运行的进程的句柄。
 * \return 返回一个NTSTATUS状态值，需要使用NT_SUCCESS宏判断操作是否成功。
 */
NTSTATUS SuspendProcessByHandle(HANDLE hProcess);

/**
 * \brief 恢复进程 
 * 
 * \param hProcess 需要恢复运行的进程的句柄
 * \return 返回一个NTSTATUS状态值，需要使用NT_SUCCESS宏判断操作是否成功。
 */
NTSTATUS ResumeProcessByHandle(HANDLE hProcess);

/**
 * \brief 使用ShellExecute执行命令
 * 
 * \param strExeFile 可执行程序
 * \param strExeParam 传递给程序的命令行参数
 * \param bShow 是否显示该程序的窗口
 * \return 成功返回TRUE,否则返回FALSE。
 */
BOOL CallCmd(const std::string strExeFile, const std::string strExeParam, BOOL bShow);

/**
 * \brief 查看其它进程的命令行参数
 * 
 * \param strProcessName 希望查看的进程名
 * \param strParam 获取到的命令行参数
 * \return 成功查找到命令行参数返回TRUE，没有找到进程名或者发生其它错误返回FALSE。
 */
BOOL PeekProcessCmdLine(const std::string strProcessName, OUT std::string& strParam);
};

/**
 * /}
 */