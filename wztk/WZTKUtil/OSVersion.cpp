#include "util/OSVersion.h"
#include <lm.h>
#pragma comment(lib, "netapi32.lib")

bool GetWinMajorMinorVersion(DWORD& major, DWORD& minor)
{
	bool bRetCode = false;
	LPBYTE pinfoRawData = 0;
	if (NERR_Success == NetWkstaGetInfo(NULL, 100, &pinfoRawData))
	{
		WKSTA_INFO_100* pworkstationInfo = (WKSTA_INFO_100*)pinfoRawData;
		major = pworkstationInfo->wki100_ver_major;
		minor = pworkstationInfo->wki100_ver_minor;
		::NetApiBufferFree(pinfoRawData);
		bRetCode = true;
	}
	return bRetCode;
}

std::string GetWindowsVersion(OSVERSIONINFOEX* pOsVer,SYSTEM_INFO* pSysInfo)
{
	std::string     winver;
	OSVERSIONINFOEX osver;
	SYSTEM_INFO     sysInfo;
	typedef void(__stdcall *GETSYSTEMINFO) (LPSYSTEM_INFO);

	__pragma(warning(push))
		__pragma(warning(disable:4996))
		memset(&osver, 0, sizeof(osver));
	osver.dwOSVersionInfoSize = sizeof(osver);
	GetVersionEx((LPOSVERSIONINFO)&osver);
	__pragma(warning(pop))
		DWORD major = 0;
	DWORD minor = 0;
	if (GetWinMajorMinorVersion(major, minor))
	{
		osver.dwMajorVersion = major;
		osver.dwMinorVersion = minor;
	}
	else if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2)
	{
		OSVERSIONINFOEXW osvi;
		ULONGLONG cm = 0;
		cm = VerSetConditionMask(cm, VER_MINORVERSION, VER_EQUAL);
		ZeroMemory(&osvi, sizeof(osvi));
		osvi.dwOSVersionInfoSize = sizeof(osvi);
		osvi.dwMinorVersion = 3;
		if (VerifyVersionInfoW(&osvi, VER_MINORVERSION, cm))
		{
			osver.dwMinorVersion = 3;
		}
	}

	GETSYSTEMINFO getSysInfo = (GETSYSTEMINFO)GetProcAddress(GetModuleHandle("kernel32.dll"), "GetNativeSystemInfo");
	if (getSysInfo == NULL)  getSysInfo = ::GetSystemInfo;
	getSysInfo(&sysInfo);

	if (osver.dwMajorVersion == 10 && osver.dwMinorVersion >= 0 && osver.wProductType != VER_NT_WORKSTATION)
		winver = "Windows 10 Server";
	if (osver.dwMajorVersion == 10 && osver.dwMinorVersion >= 0 && osver.wProductType == VER_NT_WORKSTATION)
		winver = "Windows 10";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 3 && osver.wProductType != VER_NT_WORKSTATION)
		winver = "Windows Server 2012 R2";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 3 && osver.wProductType == VER_NT_WORKSTATION)
		winver = "Windows 8.1";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2 && osver.wProductType != VER_NT_WORKSTATION)
		winver = "Windows Server 2012";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 2 && osver.wProductType == VER_NT_WORKSTATION)
		winver = "Windows 8";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1 && osver.wProductType != VER_NT_WORKSTATION)
		winver = "Windows Server 2008 R2";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1 && osver.wProductType == VER_NT_WORKSTATION)
		winver = "Windows 7";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0 && osver.wProductType != VER_NT_WORKSTATION)
		winver = "Windows Server 2008";
	if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0 && osver.wProductType == VER_NT_WORKSTATION)
		winver = "Windows Vista";
	if (osver.dwMajorVersion == 5 && 
		osver.dwMinorVersion == 2 && 
		osver.wProductType == VER_NT_WORKSTATION && 
		(sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || PROCESSOR_ARCHITECTURE_IA64 == sysInfo.wProcessorArchitecture))  
		winver = "Windows XP x64";
	if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2)   
		winver = "Windows Server 2003";
	if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1)
		winver = "Windows XP";
	if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 0)
		winver = "Windows 2000";
	if (osver.dwMajorVersion < 5)
		winver = "unknown";

	if (osver.wServicePackMajor != 0)
	{
		std::string sp;
		char buf[128] = { 0 };
		sp = " Service Pack ";
		sprintf_s(buf, sizeof(buf), "%hd", osver.wServicePackMajor);
		sp.append(buf);
		winver += sp;
	}
	if (pOsVer)
	{
		memcpy(pOsVer,&osver,sizeof(OSVERSIONINFOEX));
	}
	if (pSysInfo)
	{
		memcpy(pSysInfo,&sysInfo,sizeof(SYSTEM_INFO));
	}
	return winver;
}

BOOL IsWindows8Or8Point1()
{
	OSVERSIONINFOEX osver;
	SYSTEM_INFO     sysInfo;
	memset(&osver,0,sizeof(OSVERSIONINFOEX));
	memset(&sysInfo,0,sizeof(SYSTEM_INFO));
	std::string xxx = GetWindowsVersion(&osver,&sysInfo);
	BOOL bX64 = sysInfo.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 || PROCESSOR_ARCHITECTURE_IA64 == sysInfo.wProcessorArchitecture;
	std::string ooo = bX64?"x64":"x86";
	printf("OS Version = %s(%s)\n",xxx.c_str(),ooo.c_str());
	if (osver.dwMajorVersion == 6 && (osver.dwMinorVersion == 2 || osver.dwMinorVersion == 3))
	{
		printf("The Os is win8 or win8.1!\n");
		return TRUE;
	}else{
		printf("The Os is Not win8 or win8.1!\n");
		return FALSE;
	}
}


typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

BOOL isWow64()
{
#ifdef _WIN64
	return TRUE;
#else
	BOOL bIsWow64 = FALSE;
	LPFN_ISWOW64PROCESS  fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
	if (fnIsWow64Process) {
		fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
	}
	return bIsWow64;
#endif // _WIN64
}

BOOL IsOsWindowsVistaorLater()
{
	OSVERSIONINFO osvi;
	BOOL bIsWindowsVistaorLater;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	GetVersionEx(&osvi);

	bIsWindowsVistaorLater = (osvi.dwMajorVersion > 5);
	//return TRUE;
	return bIsWindowsVistaorLater;
}
