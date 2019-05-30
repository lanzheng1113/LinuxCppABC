/**
 * \copyright (C)2016 万兆天空
 * \file ProcessUtils.cpp 
 * \brief 实现一些和读取进程信息有关的函数。\see ProcessUtils.h 
 * \author fanduoduo
 * \date 2016/11/10
 * \version 1.0
 */

#include "util/ProcessUtils.h"
#include<Tlhelp32.h>
#include <Psapi.h>
#include "util/StringEx.h"
#pragma comment (lib,"Psapi.lib") 
#include "util/FreeAndClose.h"
namespace Process
{
	BOOL IsProcessExists(DWORD pid)
	{
		HANDLE myhProcess;
		PROCESSENTRY32 mype;
		mype.dwSize = sizeof(PROCESSENTRY32);
		BOOL mybRet;
		//进行进程快照
		myhProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //快照所有进程
		//开始进程查找
		mybRet = Process32First(myhProcess, &mype);
		//循环比较，得出ProcessID
		while (mybRet)
		{
			if (mype.th32ProcessID == pid)
			{
				return TRUE;
			}			
			mybRet = Process32Next(myhProcess, &mype);
		}
		return FALSE;
	}

	std::string GetPathFromProcessId(DWORD pid)
	{
		std::string   path;
		WCHAR szBuf[MAX_PATH] = { 0 };
		HANDLE h_Process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
		if (h_Process)
		{
			GetProcessImageFileNameW(h_Process, szBuf, MAX_PATH * sizeof(WCHAR));
			path = DosDevicePath2LogicalPath(szBuf);
		}
		return path;
	}

	DWORD KillProcess( const std::string& strProcessName )
	{
		PROCESSENTRY32 mype;
		mype.dwSize = sizeof(PROCESSENTRY32);
		HANDLE SnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (SnapshotHandle == INVALID_HANDLE_VALUE)
		{
			return 0;
		}
		DWORD dwRet = 0;
		if (Process32First(SnapshotHandle,&mype))
		{
			std::string upperInputName = String(strProcessName).toUpperCase();
			do 
			{
				if(String(mype.szExeFile).toUpperCase() == upperInputName){
					HANDLE h_Process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE, mype.th32ProcessID);
					if (h_Process)
					{
						dwRet ++;
						TerminateProcess(h_Process,1);
						WaitForSingleObject(h_Process,1000);
						CloseHandle(h_Process);
					}
				}
			} while (Process32Next(SnapshotHandle, &mype));
		}
		return dwRet;
	}

	DWORD GetProcessIdByName( const std::string& strProcessName )
	{
		PROCESSENTRY32 mype;
		mype.dwSize = sizeof(PROCESSENTRY32);
		HANDLE SnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (SnapshotHandle == INVALID_HANDLE_VALUE)
		{
			return 0;
		}
		DWORD dwRet = 0;
		if (Process32First(SnapshotHandle,&mype))
		{
			std::string upperInputName = String(strProcessName).toUpperCase();
			do 
			{
				if(String(mype.szExeFile).toUpperCase() == upperInputName){
					dwRet = mype.th32ProcessID;
					break;
				}
			} while (Process32Next(SnapshotHandle, &mype));
		}
		return dwRet;
	}

	DWORD GetProcessIdLikeName(const std::string& strProcessName, OUT std::string& processName)
	{
		processName = "";
		PROCESSENTRY32 mype;
		mype.dwSize = sizeof(PROCESSENTRY32);
		HANDLE SnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (SnapshotHandle == INVALID_HANDLE_VALUE)
		{
			return 0;
		}

		DWORD dwRet = 0;
		if (Process32First(SnapshotHandle, &mype))
		{
			std::string upperInputName = String(strProcessName).toUpperCase();
			do
			{
				if (String(mype.szExeFile).toUpperCase() == upperInputName) {
					dwRet = mype.th32ProcessID;
					processName = mype.szExeFile;
					break;
				}
			} while (Process32Next(SnapshotHandle, &mype));
		}

		// 如果没有完全匹配的，那么查找一个像它的。
		if (dwRet == 0)
		{
			if (Process32First(SnapshotHandle, &mype))
			{
				std::string upperInputName = String(strProcessName).toUpperCase();
				do
				{
					if (String(mype.szExeFile).toUpperCase().find(upperInputName) != std::string::npos)
					{
						dwRet = mype.th32ProcessID;
						processName = mype.szExeFile;
						break;
					}
				} while (Process32Next(SnapshotHandle, &mype));
			}
		}

		CloseHandle(SnapshotHandle);
		return dwRet;
	}

	DWORD GetProcessIdFromPath(const std::string& FilePath)
	{
		HANDLE myhProcess;
		PROCESSENTRY32 mype;
		mype.dwSize = sizeof(PROCESSENTRY32);
		BOOL mybRet;
		//进行进程快照
		myhProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //快照所有进程
		if (myhProcess)
		{
			//开始进程查找
			mybRet = Process32First(myhProcess, &mype);
			//循环比较，得出ProcessID
			while (mybRet)
			{
				std::string   path;
				WCHAR szBuf[MAX_PATH] = { 0 };
				HANDLE h_Process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, mype.th32ProcessID);
				if (!h_Process)
				{
					mybRet = Process32Next(myhProcess, &mype);
					continue;
				}
				GetProcessImageFileNameW(h_Process, szBuf, MAX_PATH * sizeof(WCHAR));
				path = DosDevicePath2LogicalPath(szBuf);

				if (String(FilePath).toUpperCase() == String(path).toUpperCase())
					return mype.th32ProcessID;
				else
					mybRet = Process32Next(myhProcess, &mype);
			}
			CloseHandle(myhProcess);
		}
		return 0;
	}

	std::string DosDevicePath2LogicalPath(const wchar_t* lpszDosPath)
	{
		std::string strResult;
		// Translate path with device name to drive letters.
		// 通过GetLogicalDriveStringsW获取所有的盘符CDEFG...
		wchar_t szTemp[MAX_PATH];
		szTemp[0] = '\0';
		if (lpszDosPath == NULL || !GetLogicalDriveStringsW(_countof(szTemp) - 1, szTemp))
		{
			return strResult;
		}

		wchar_t szName[MAX_PATH];
		wchar_t szDrive[3] = L" :";
		BOOL bFound = FALSE;
		wchar_t* p = szTemp;
		do {
			// Copy the drive letter to the template string
			// 遍历系统上所有的盘符，将查询逻辑路径对应的DOS设备路径（C: --> \\Device\\HarddiskVolume1）
			*szDrive = *p;
			// Look up each device name
			if (QueryDosDeviceW(szDrive, szName, _countof(szName))) 
			{
				UINT uNameLen = (UINT)wcslen(szName);
				if (uNameLen < MAX_PATH)
				{
					// 如果输入的DOS设备路径以该盘符的DOS设备路径开头
					bFound = _wcsnicmp(lpszDosPath, szName, uNameLen) == 0;
					if (bFound) 
					{
						// Reconstruct pszFilename using szTemp
						// Replace device path with DOS path
						// 用对应的逻辑路径替换DOS设备路径（\\Device\\HarddiskVolume1\A.sys --> C:\A.sys）
						wchar_t szTempFile[MAX_PATH];
						swprintf_s(szTempFile, L"%s%s", szDrive, lpszDosPath + uNameLen);
						strResult = String::fromStdWString(szTempFile);
					}
				}
			}
			// Go to the next NULL character.
			while (*p++);
		} while (!bFound && *p); // end of string
		return strResult;
	}

	HANDLE OpenProcessByPName(const std::string& strProcessName)
	{
		HANDLE hRet = INVALID_HANDLE_VALUE;
		PROCESSENTRY32 mype;
		mype.dwSize = sizeof(PROCESSENTRY32);
		HANDLE SnapshotHandle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (SnapshotHandle == INVALID_HANDLE_VALUE)
		{
			return 0;
		}
		DWORD dwRet = 0;
		if (Process32First(SnapshotHandle, &mype))
		{
			do
			{
				std::string strProcessName = mype.szExeFile;
				if (String(strProcessName).toUpperCase() == String(mype.szExeFile).toUpperCase()) {
					HANDLE h_Process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE, mype.th32ProcessID);
					if (h_Process)
					{
						hRet = h_Process;
						break;
					}
				}
			} while (Process32Next(SnapshotHandle, &mype));
		}
		CloseHandle(SnapshotHandle);
		return hRet;
	}

	BOOL SetSeDebugPrivilege(BOOL bEnablePrivilege)
	{
		BOOL bRet = 0;
		DWORD dwErr = 0;

		HANDLE hToken = NULL;

		TOKEN_PRIVILEGES tp;
		LUID luid;
		TOKEN_PRIVILEGES tpPrevious;
		DWORD cbPrevious;
		HANDLE hThread = GetCurrentThread();
		LPCTSTR Privilege = SE_DEBUG_NAME;

		do
		{
			//OpenThreadToken  
			{
				if (OpenThreadToken(hThread, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken))
				{
				}
				else
				{
					dwErr = GetLastError();
					if (dwErr != ERROR_NO_TOKEN)
					{
						bRet = -1;
						break;
					}

					if (!ImpersonateSelf(SecurityImpersonation))
					{
						dwErr = GetLastError();
						bRet = -2;
						break;
					}

					if (!OpenThreadToken(hThread, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken))
					{
						dwErr = GetLastError();
						bRet = -3;
						break;
					}
				}
			}

			//LookupPrivilegeValue  
			{
				if (!LookupPrivilegeValue(NULL, Privilege, &luid))
				{
					dwErr = GetLastError();
					bRet = -4;
					break;
				}

				// first pass.  get current privilege setting  
				tp.PrivilegeCount = 1;
				tp.Privileges[0].Luid = luid;
				tp.Privileges[0].Attributes = 0;
				if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES),
					&tpPrevious, &cbPrevious))
				{
					dwErr = GetLastError();
					bRet = -5;
					break;
				}

				// second pass.  set privilege based on previous setting  
				tpPrevious.PrivilegeCount = 1;
				tpPrevious.Privileges[0].Luid = luid;
				if (bEnablePrivilege)
				{
					tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
				}
				else
				{
					tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED &
						tpPrevious.Privileges[0].Attributes);
				}

				if (!AdjustTokenPrivileges(hToken, FALSE, &tpPrevious, cbPrevious, NULL, NULL))
				{
					dwErr = GetLastError();
					bRet = -6;
					break;
				}
			}

			bRet = TRUE;
			dwErr = 0;
		} while (0);

		if (hToken)
		{
			CloseHandle(hToken);
			hToken = NULL;
		}
		return bRet;
	}

#define STATUS_UNSUCCESSFUL              ((NTSTATUS)0xC0000001L)
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)
#define STATUS_NAME_TOO_LONG             ((NTSTATUS)0xC0000106L)
#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

	//暂停进程
	NTSTATUS SuspendProcessById(DWORD ProcessId)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		HANDLE ProcessHandle;
		typedef NTSTATUS(__stdcall *fnZwSuspendProcess)(IN HANDLE Handle);
		static fnZwSuspendProcess ZwSuspendProcess = (fnZwSuspendProcess)GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwSuspendProcess");

		ProcessHandle = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, ProcessId);
		if (ProcessHandle)
		{

			status = ZwSuspendProcess(ProcessHandle);
			CloseHandle(ProcessHandle);
		}

		return status;
	}

	//激活进程
	NTSTATUS ResumeProcessById(DWORD ProcessId)
	{

		NTSTATUS status = STATUS_UNSUCCESSFUL;
		HANDLE ProcessHandle;
		typedef NTSTATUS(__stdcall *fnZwResumeProcess)(IN HANDLE Handle);
		static fnZwResumeProcess ZwResumeProcess = (fnZwResumeProcess)GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwResumeProcess");

		ProcessHandle = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, ProcessId);
		if (ProcessHandle)
		{

			status = ZwResumeProcess(ProcessHandle);
			CloseHandle(ProcessHandle);
		}

		return status;
	}

	NTSTATUS SuspendProcessByHandle(HANDLE hProcess)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		typedef NTSTATUS(__stdcall *fnZwResumeProcess)(IN HANDLE Handle);
		static fnZwResumeProcess ZwResumeProcess = (fnZwResumeProcess)GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwResumeProcess");
		status = ZwResumeProcess(hProcess);
		return status;
	}

	NTSTATUS ResumeProcessByHandle(HANDLE hProcess)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		typedef NTSTATUS(__stdcall *fnZwResumeProcess)(IN HANDLE Handle);
		static fnZwResumeProcess ZwResumeProcess = (fnZwResumeProcess)GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwResumeProcess");
		status = ZwResumeProcess(hProcess);
		return status;
	}

	BOOL CallCmd(const std::string strExeFile, const std::string strExeParam, BOOL bShow)
	{
		if (strExeFile.size() < 3)
			return FALSE;

		int iShow;
		if (!bShow)
		{
			iShow = SW_HIDE;
		}
		else
		{
			iShow = SW_SHOW;
		}
		SHELLEXECUTEINFOA ShExecInfo = { 0 };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFOA);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.hwnd = NULL;
		ShExecInfo.lpVerb = NULL;
		ShExecInfo.lpFile = strExeFile.c_str();
		ShExecInfo.lpParameters = strExeParam.c_str();
		ShExecInfo.lpDirectory = NULL;
		ShExecInfo.nShow = iShow;
		ShExecInfo.hInstApp = NULL;
		ShellExecuteExA(&ShExecInfo);
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
		DWORD dwExitCode = -1;
		if (GetExitCodeProcess(ShExecInfo.hProcess, &dwExitCode))
		{
			if (dwExitCode == 0)
				return TRUE;
		}
		return FALSE;
	}

	typedef NTSTATUS(NTAPI *_NtQueryInformationProcess)(
		HANDLE ProcessHandle,
		DWORD ProcessInformationClass,
		PVOID ProcessInformation,
		DWORD ProcessInformationLength,
		PDWORD ReturnLength
		);

	typedef struct _UNICODE_STRING
	{
		USHORT Length;
		USHORT MaximumLength;
		PWSTR Buffer;
	} UNICODE_STRING, *PUNICODE_STRING;

	typedef struct _PROCESS_BASIC_INFORMATION
	{
		LONG ExitStatus;
		PVOID PebBaseAddress;
		ULONG_PTR AffinityMask;
		LONG BasePriority;
		ULONG_PTR UniqueProcessId;
		ULONG_PTR ParentProcessId;
	} PROCESS_BASIC_INFORMATION, *PPROCESS_BASIC_INFORMATION;

	static PVOID GetPebAddress(HANDLE ProcessHandle)
	{
		_NtQueryInformationProcess NtQueryInformationProcess =
			(_NtQueryInformationProcess)GetProcAddress(
				GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
		PROCESS_BASIC_INFORMATION pbi;
		NtQueryInformationProcess(ProcessHandle, 0, &pbi, sizeof(pbi), NULL);
		return pbi.PebBaseAddress;
	}


	BOOL PeekProcessCmdLine(const std::string strProcessName, OUT std::string& strParam)
	{
		BOOL bRet = FALSE;
		int pid;
		HANDLE processHandle;
		PVOID pebAddress;
		PVOID rtlUserProcParamsAddress;
		UNICODE_STRING commandLine;
		WCHAR *commandLineContents;

		
		pid = GetProcessIdByName(strProcessName);
		if (pid == 0)
		{
			return FALSE;
		}

		if ((processHandle = OpenProcess(
			PROCESS_QUERY_INFORMATION | /* required for NtQueryInformationProcess */
			PROCESS_VM_READ, /* required for ReadProcessMemory */
			FALSE, pid)) == 0)
		{
			printf("Could not open process!\n");
			return FALSE;
		}
		HandleCloser h(processHandle);
		pebAddress = GetPebAddress(processHandle);

		/* get the address of ProcessParameters */
		if (!ReadProcessMemory(processHandle, (PCHAR)pebAddress + 0x10,
			&rtlUserProcParamsAddress, sizeof(PVOID), NULL))
		{
			printf("Could not read the address of ProcessParameters!\n");
			return FALSE;
		}

		/* read the CommandLine UNICODE_STRING structure */
		if (!ReadProcessMemory(processHandle, (PCHAR)rtlUserProcParamsAddress + 0x40,
			&commandLine, sizeof(commandLine), NULL))
		{
			printf("Could not read CommandLine!\n");
			return FALSE;
		}

		/* allocate memory to hold the command line */
		commandLineContents = new WCHAR [commandLine.Length];
		ZeroMemory(commandLineContents, commandLine.Length * sizeof(WCHAR));
		DataArrayDeleteAgent<WCHAR> deleter(commandLineContents);

		/* read the command line */
		if (!ReadProcessMemory(processHandle, commandLine.Buffer,
			commandLineContents, commandLine.Length, NULL))
		{
			printf("Could not read the command line string!\n");
			return FALSE;
		}

		/* print it */
		/* the length specifier is in characters, but commandLine.Length is in bytes */
		/* a WCHAR is 2 bytes */
		printf("%.*S\n", commandLine.Length / 2, commandLineContents);

		strParam = String::fromStdWString(commandLineContents);
		return TRUE;
	}
};
