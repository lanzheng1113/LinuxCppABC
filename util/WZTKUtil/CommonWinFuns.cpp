#include <Windows.h>
#include <tchar.h>
#include "util/CommonWinFuns.h"
#include "util/RegKeyCloseHelper.h"
#include <sstream>
#include <Sddl.h>
#include <Aclapi.h>
#include "util/OSVersion.h"

#ifndef COMMLOG
#define COMMLOG
#endif

bool GetAccountSid(LPSTR AccountName, OUT PSID *Sid)
{
	PSID pSID = NULL;
	DWORD cbSid = 0;
	LPSTR DomainName = NULL;
	DWORD cbDomainName = 0;
	SID_NAME_USE SIDNameUse;
	BOOL  bDone = FALSE;
	try
	{
		if(!LookupAccountName(NULL,
			AccountName,
			pSID,
			&cbSid,
			DomainName,
			&cbDomainName,
			&SIDNameUse))
		{
			printf("LookupAccountName GetLastError() = %d,and cbSid = %d\n",GetLastError(),cbSid);
			if (0 == cbSid){
				throw std::logic_error("LookupAccountName调用失败SID结构体大小=0");
			}

			pSID = LocalAlloc(0,cbSid);
			DomainName = (LPSTR)new CHAR[cbDomainName];
			if(!pSID || !DomainName){
				throw std::bad_alloc();
			}
			if(!LookupAccountName(NULL,
				AccountName,
				pSID,
				&cbSid,
				DomainName,
				&cbDomainName,
				&SIDNameUse))
			{
				std::stringstream ss;
				ss << "LookupAccountName调用失败。GetLastError()=" << GetLastError();
				throw std::logic_error(ss.str().c_str());
			}
			bDone = TRUE;
		}
	}
	catch(std::logic_error e)
	{
		//@TODO:在这里收集throw出来的异常信息，写入到日志中。注意这里不能做用__try,__finally做的资源释放的事情。
		//exception是做错误处理,finally做的是资源释放的事，这是两码事。
		//类似：LogDebug("Error :%s",exception.desc());
		printf("%s\n", e.what());
	}

	if(DomainName)
	{
		delete [] DomainName;
		DomainName = NULL;
	}

	if(!bDone && pSID)
	{
		LocalFree(pSID);
		*Sid = NULL;
		DomainName = NULL;
	}

	if(bDone)
	{
		*Sid = pSID;
	}else{
		*Sid = NULL;
	}

	return bDone?true:false;
}


std::string GetHKCURealPath()
{
	std::string ret = "";
	CHAR szBuf[MAX_PATH] = "";
	DWORD dwRet = MAX_PATH;
	BOOL bGetNameOfUser = GetUserName(szBuf, &dwRet);
	if (!bGetNameOfUser)
	{
		return ret;
	}

	PSID pSid = NULL;
	bool bGetSid = GetAccountSid(szBuf, &pSid);
	if (!bGetSid)
	{
		return ret;
	}

	LPSTR sid = NULL;
	//\note:
	// About sid parameter of ConvertSidToStringSid function:
	// It is a pointer to a variable that receives a pointer to a null-terminated SID string. 
	// To free the returned buffer, call the LocalFree function.
	ConvertSidToStringSid(pSid, &sid); //从结构体中得到sid串
	if (pSid)
	{
		LocalFree(pSid);
		pSid = NULL;
	}

	if (sid)
	{
		ret = sid;
		LocalFree(sid);
		sid = NULL;
	}
	return ret;
}


BOOL SetPrivilege( HANDLE hToken, LPCSTR lpszPrivilege, BOOL bEnablePrivilege )
{
	LUID luid;
	if( !LookupPrivilegeValue( NULL, lpszPrivilege, &luid ) )
	{
		GetLastError();
		return FALSE;
	}

	TOKEN_PRIVILEGES tp;
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if( bEnablePrivilege )
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	if( !AdjustTokenPrivileges( hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES) NULL, (PDWORD) NULL) )
	{ 
		GetLastError();
		return FALSE;
	}

	if( GetLastError()==ERROR_NOT_ALL_ASSIGNED )
		return FALSE;

	return TRUE;
}

DWORD TakeObjectsOwnerShip( HKEY hPrefKey, LPCSTR lpczSubKey )
{
	std::string strObjName;// = _T("CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\FileExts\\.html\\UserChoice");
	if( HKEY_CLASSES_ROOT==hPrefKey )
		strObjName = "CLASSES_ROOT\\";
	else if( HKEY_CURRENT_USER==hPrefKey )
		strObjName = "CURRENT_USER\\";
	else if( HKEY_LOCAL_MACHINE==hPrefKey )
		strObjName = "MACHINE\\";
	else
		return ERROR_INVALID_PARAMETER;
	strObjName += lpczSubKey;

	DWORD dwRes = ERROR_SUCCESS;

	PSID pSIDAdmin = NULL;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
	AllocateAndInitializeSid( &SIDAuthNT, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &pSIDAdmin ); 

	HANDLE hToken = NULL;
	// Create a new ACL that merges the new ACE into the existing DACL.
	PACL pACL = NULL;
	EXPLICIT_ACCESS ea[2] = { 0 };
	ea[0].grfAccessPermissions = GENERIC_ALL;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[0].Trustee.ptstrName = (LPSTR) pSIDAdmin;

	ea[1].grfAccessPermissions = GENERIC_READ;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance = NO_INHERITANCE;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_NAME;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_USER;
	ea[1].Trustee.ptstrName = (LPSTR)("EVERYONE");

	dwRes = SetEntriesInAcl( sizeof(ea)/sizeof(EXPLICIT_ACCESS), ea, NULL, &pACL );
	if( ERROR_SUCCESS==dwRes )
	{
		if( OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken ) )
			if( !SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, TRUE) )
				dwRes = ::GetLastError();
		/*else
		dwRes = ::GetLastError();*/
	}

	if( ERROR_SUCCESS==dwRes )
		dwRes = SetNamedSecurityInfo( (LPSTR)(LPCSTR)strObjName.c_str(), SE_REGISTRY_KEY, OWNER_SECURITY_INFORMATION, pSIDAdmin, NULL, NULL, NULL );

	if( ERROR_SUCCESS==dwRes )
		if( !SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, FALSE) )
			dwRes = ::GetLastError();

	CloseHandle( hToken );
	FreeSid( pSIDAdmin );

	if( ERROR_SUCCESS==dwRes )
		dwRes = SetNamedSecurityInfo( (LPSTR)(LPCSTR)strObjName.c_str(), SE_REGISTRY_KEY, DACL_SECURITY_INFORMATION, NULL, NULL, pACL, NULL );

	LocalFree( (HLOCAL) pACL ); 

	return dwRes;
}

bool GetDefaultBrowserLaunchCommand(std::string& default_browse)
{
	bool    bRes            = false;
	HKEY    hKey1            = 0;
	HKEY	hKey2			= 0;
	CHAR    szData[1024]    = {0};
	DWORD    dwDataSize        = 0;
	//
	// Vista+ case
	//
	if ( IsOsWindowsVistaorLater() )
	{
		if ( ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, 
			TEXT("Software\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice"),
			0, 
			KEY_QUERY_VALUE, 
			&hKey1) )
		{
			CRegKeyCloseHelper helper1(hKey1);
			//
			// Vista+ does not always have the registry entry we use in the WinXP case (?)
			// So we do a workaround:
			//        1. Read the current browser Progid value from HKCU (current user!)
			//        2. Use this Progid to get the browser command line from global HKCR
			///          (as every browser in the system writes its command line into HKCR)
			//
			dwDataSize    = ARRAYSIZE(szData) * sizeof(szData[0]);
			if (ERROR_SUCCESS != RegQueryValueEx(hKey1, "Progid", 0, 0, (LPBYTE)&szData, &dwDataSize))
			{
				COMMLOG("CommFunLib","[Vista][获取默认浏览器]查询http\\UserChoice的Progid失败了");
				return bRes;
			}
			if (!dwDataSize)
			{
				COMMLOG("CommFunLib","[Vista][获取默认浏览器]http\\UserChoice的Progid的值为空");
				return bRes;
			}
			hKey1 = 0;

			strcat_s(szData, ARRAYSIZE(szData), "\\shell\\open\\command");
			if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CLASSES_ROOT, szData, 0, KEY_QUERY_VALUE, &hKey2))                                   // Using HKCR (!)
			{
				COMMLOG("CommFunLib","[Vista][获取默认浏览器]无法打开HKEY_CLASSES_ROOT\\%s",szData);
				return bRes;
			}
		}else{
			COMMLOG("CommFunLib","[Vista][获取默认浏览器]无法打开HKEY_CURRENT_USERSoftware\\Microsoft\\Windows\\Shell\\Associations\\UrlAssociations\\http\\UserChoice,尝试HKEY_CLASSES_ROOT\\http\\shell\\open\\command");
			if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CLASSES_ROOT, "http\\shell\\open\\command", 0, KEY_QUERY_VALUE, &hKey2))        // Using HKCU in WinXP (!)
			{
				COMMLOG("CommFunLib","[Vista][获取默认浏览器]无法打开HKEY_CLASSES_ROOT\\http\\shell\\open\\command");
				return bRes;
			}
		}
	}
	else
	{
		//
		// WinXP case
		//
		if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_CLASSES_ROOT, "http\\shell\\open\\command", 0, KEY_QUERY_VALUE, &hKey2))        // Using HKCU in WinXP (!)
		{
			COMMLOG("CommFunLib","[XP][获取默认浏览器]无法打开HKEY_CLASSES_ROOT\\Software\\Classes\\http\\shell\\open\\command");
			return bRes;
		}
	}

	if (hKey2)
	{
		CRegKeyCloseHelper helper1(hKey2);
		if (ERROR_SUCCESS != RegQueryValueEx(hKey2, 0, 0, 0, 0, &dwDataSize))
		{
			// Get size in bytes of the default key's value
			COMMLOG("CommFunLib","[获取默认浏览器]打开查询命令行的子键失败，无法继续。");
			return bRes;
		}
		// Buf size in chars +3 chars to reserve the space for an optional " %1" param and +1 char for \0 terminator
		DWORD nMaxSize    = dwDataSize/sizeof(CHAR) + 3 + 1;// 
		CHAR* TextPtr = new CHAR[nMaxSize];
		if (!TextPtr)
		{
			COMMLOG("CommFunLib","[获取默认浏览器]内存分配失败！");
			delete [] TextPtr;
			return bRes;
		}
		memset(TextPtr,0,sizeof(CHAR)*nMaxSize);

		if (ERROR_SUCCESS != RegQueryValueEx(hKey2, 0, 0, 0, (LPBYTE)TextPtr, &dwDataSize))
		{
			COMMLOG("CommFunLib", "[获取默认浏览器]查询命令行子键的值失败！");
			delete [] TextPtr;
			return bRes;
		}

		if (!strstr(TextPtr, "%1"))
		{
			strcat_s(TextPtr, nMaxSize, " %1");
		}
		// Add a URL placeholder (it is missing in IE) to the end of the command line
		bRes = true;
		default_browse = TextPtr;
		COMMLOG("CommFunLib","[获取默认浏览器]OK！默认浏览器为%s！",default_browse.c_str());
		delete [] TextPtr;
	}
	else
	{
		COMMLOG("CommFunLib","[获取默认浏览器]子键为空，无法继续。");
	}

	return bRes;
}

std::wstring GetIEPath()
{
	if (IsOsWindowsVistaorLater())
	{
		LONG cbSize = 0;
		HKEY hKeySub = 0;
		std::wstring new_reg_path = L"IE.HTTP\\shell\\open\\command";
		LSTATUS retCode = RegOpenKeyExW(HKEY_CLASSES_ROOT,new_reg_path.c_str(),0,KEY_READ,&hKeySub);
		if (retCode == ERROR_SUCCESS)
		{
			CRegKeyCloseHelper closer(hKeySub); //在离开这个scope时自动关闭这个键！
			//成功打开子键
			//查询默认键
			DWORD cbLen = 0;
			LSTATUS retStatus = RegQueryValueEx(hKeySub,TEXT(""),NULL,NULL,NULL,&cbLen);
			if (retStatus != ERROR_SUCCESS && retStatus != ERROR_MORE_DATA)
			{
				COMMLOG("CommFunLib","查询%S下的默认键值时发生了错误:%d",new_reg_path.c_str(),GetLastError());
				return L"";
			}
			else
			{
				if (cbLen == 0)
				{
					COMMLOG("CommFunLib","查询此键下的默认键值时发生了错误：此值为空！");
					return L"";
				}

				DWORD need_alloc_count = cbLen+2;
				BYTE* pbuf = new BYTE[need_alloc_count];
				if (!pbuf){
					return L"";
				}
				memset(pbuf,0,need_alloc_count);
				retStatus = RegQueryValueEx(hKeySub,TEXT(""),NULL,NULL,pbuf,&need_alloc_count);
				if (retStatus != ERROR_SUCCESS)
				{
					COMMLOG("CommFunLib","查询此键下的默认键值(2)时发生了错误: %d",GetLastError());
					delete [] pbuf;
					return L"";
				}
				COMMLOG("CommFunLib","成功获取[(默认)]=%S",(WCHAR*)pbuf);
				std::wstring path_with_nohome = std::wstring((WCHAR*)pbuf);
				std::wstring::size_type pos1 = 0;
				std::wstring::size_type pos2 = 0;

				pos1 = path_with_nohome.find(L'\"');
				if (pos1 == std::wstring::npos)
				{
					delete [] pbuf;
					return path_with_nohome;
				}
				pos2 = path_with_nohome.find(L'\"',pos1+1);
				if (pos2 == std::wstring::npos)
				{
					delete [] pbuf;
					return path_with_nohome;
				}

				if (pos2-pos1 <= 2)
				{
					delete [] pbuf;
					return path_with_nohome;
				}

				std::wstring wpath_actully = path_with_nohome.substr(pos1+1,pos2-pos1-1);
				delete [] pbuf;
				return wpath_actully;
			}
		}
		else
		{
			return L"";
		}
	}else{
		//xp
		wchar_t sysDir[260];  
		GetSystemDirectoryW(sysDir, 260);
		sysDir[1] = 0;
		std::wstring wstr_sysDir = sysDir;
		wstr_sysDir += L":\\Program Files\\Internet Explorer\\iexplore.exe";
		return wstr_sysDir;
	}
	return L"";
}


BOOL RegistrySetValueDowrd( HKEY hKeyRoot, const char* regPath, const char* keyName,DWORD val, BOOL WriteTo64bitRegPath )
{
	HKEY hKey = 0;
	DWORD keyMask = 0;

	if (WriteTo64bitRegPath)
	{
		keyMask = KEY_WOW64_64KEY;
	}

	bool bxx = true;
	if (ERROR_SUCCESS != ::RegOpenKeyExA(hKeyRoot, regPath,0, KEY_WRITE|keyMask, &hKey))
	{
		if (ERROR_SUCCESS != ::RegCreateKeyA(hKeyRoot, regPath, &hKey))
		{
			bxx = false;
		}
	}
	if (bxx)
	{
		LSTATUS rs = ::RegSetValueExA(hKey, keyName, 0, REG_DWORD, (const BYTE*)&val, sizeof(DWORD));
		::RegCloseKey(hKey);
		return rs == ERROR_SUCCESS;
	}
	else
	{
		return FALSE;
	}
}


BOOL RegistrySetValueString( HKEY hKeyRoot,const char* regPath,const char* keyName, const char* val,BOOL WriteTo64bitRegPath,bool isExpandStrType)
{
	HKEY hKey = 0;
	DWORD keyMask = 0;
	if (WriteTo64bitRegPath)
	{
		keyMask = KEY_WOW64_64KEY;
	}
	bool bxx = true;
	if (ERROR_SUCCESS != ::RegOpenKeyExA(hKeyRoot, regPath,0, KEY_WRITE|keyMask, &hKey))
	{
		if (ERROR_SUCCESS != ::RegCreateKeyA(hKeyRoot, regPath, &hKey)){
			bxx = false;
		}
	}
	if (bxx)
	{
		//printf("key = %x,name=%s,value=%s",hKey,keyName,xurl);
		DWORD dwType = isExpandStrType ? REG_EXPAND_SZ : REG_SZ;
		LSTATUS rs = ::RegSetValueExA( hKey, keyName, 0, dwType, (const BYTE*)val, strlen(val) );
		::RegCloseKey( hKey );
		return ERROR_SUCCESS == rs;
	}
	else
	{
		printf( "failed!with error %d", GetLastError() );
		return FALSE;
	}
}

BOOL TakeOwnershipOfFilePath(LPSTR lpszOwnFile) 
{
	BOOL bRetval = FALSE;
	HANDLE hToken = NULL; 
	PSID pSIDAdmin = NULL;
	PSID pSIDEveryone = NULL;
	PACL pACL = NULL;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld =
		SECURITY_WORLD_SID_AUTHORITY;
	SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
	const int NUM_ACES  = 2;
	EXPLICIT_ACCESS ea[NUM_ACES];
	DWORD dwRes;

	// Specify the DACL to use.
	// Create a SID for the Everyone group.
	if (!AllocateAndInitializeSid(&SIDAuthWorld, 1,
		SECURITY_WORLD_RID,
		0,
		0, 0, 0, 0, 0, 0,
		&pSIDEveryone)) 
	{
		printf("AllocateAndInitializeSid (Everyone) error %u\n",
			GetLastError());
		goto Cleanup;
	}

	// Create a SID for the BUILTIN\Administrators group.
	if (!AllocateAndInitializeSid(&SIDAuthNT, 2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pSIDAdmin)) 
	{
		printf("AllocateAndInitializeSid (ADMIN) error %u\n", GetLastError());
		goto Cleanup;
	}

	ZeroMemory(&ea, NUM_ACES * sizeof(EXPLICIT_ACCESS));

	// Set read access for Everyone.
	ea[0].grfAccessPermissions = GENERIC_READ;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName = (LPTSTR) pSIDEveryone;

	// Set full control for Administrators.
	ea[1].grfAccessPermissions = GENERIC_ALL;
	ea[1].grfAccessMode = SET_ACCESS;
	ea[1].grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
	ea[1].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[1].Trustee.TrusteeType = TRUSTEE_IS_GROUP;
	ea[1].Trustee.ptstrName = (LPTSTR) pSIDAdmin;

	if (ERROR_SUCCESS != SetEntriesInAcl(NUM_ACES,
		ea,
		NULL,
		&pACL))
	{
		printf("Failed SetEntriesInAcl\n");
		goto Cleanup;
	}

	// Try to modify the object's DACL.
	dwRes = SetNamedSecurityInfo(
		lpszOwnFile,                 // name of the object
		SE_FILE_OBJECT,              // type of object
		DACL_SECURITY_INFORMATION,   // change only the object's DACL
		NULL, NULL,                  // do not change owner or group
		pACL,                        // DACL specified
		NULL);                       // do not change SACL

	if (ERROR_SUCCESS == dwRes) 
	{
		printf("Successfully changed DACL\n");
		bRetval = TRUE;
		// No more processing needed.
		goto Cleanup;
	}
	if (dwRes != ERROR_ACCESS_DENIED)
	{
		//printf("First SetNamedSecurityInfo call failed: %u\n",
		//	dwRes); 
		goto Cleanup;
	}

	// If the preceding call failed because access was denied, 
	// enable the SE_TAKE_OWNERSHIP_NAME privilege, create a SID for 
	// the Administrators group, take ownership of the object, and 
	// disable the privilege. Then try again to set the object's DACL.

	// Open a handle to the access token for the calling process.
	if (!OpenProcessToken(GetCurrentProcess(), 
		TOKEN_ADJUST_PRIVILEGES, 
		&hToken)) 
	{
		printf("OpenProcessToken failed: %u\n", GetLastError()); 
		goto Cleanup; 
	} 

	// Enable the SE_TAKE_OWNERSHIP_NAME privilege.
	if (!SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, TRUE)) 
	{
		printf("You must be logged on as Administrator.\n");
		goto Cleanup; 
	}

	// Set the owner in the object's security descriptor.
	dwRes = SetNamedSecurityInfo(
		lpszOwnFile,                 // name of the object
		SE_FILE_OBJECT,              // type of object
		OWNER_SECURITY_INFORMATION,  // change only the object's owner
		pSIDAdmin,                   // SID of Administrator group
		NULL,
		NULL,
		NULL); 

	if (dwRes != ERROR_SUCCESS) 
	{
		printf("Could not set owner. Error: %u\n", dwRes); 
		goto Cleanup;
	}

	// Disable the SE_TAKE_OWNERSHIP_NAME privilege.
	if (!SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, FALSE)) 
	{
		printf("Failed SetPrivilege call unexpectedly.\n");
		goto Cleanup;
	}

	// Try again to modify the object's DACL,
	// now that we are the owner.
	dwRes = SetNamedSecurityInfo(
		lpszOwnFile,                 // name of the object
		SE_FILE_OBJECT,              // type of object
		DACL_SECURITY_INFORMATION,   // change only the object's DACL
		NULL, NULL,                  // do not change owner or group
		pACL,                        // DACL specified
		NULL);                       // do not change SACL

	if (dwRes == ERROR_SUCCESS)
	{
		printf("Successfully changed DACL\n");
		bRetval = TRUE; 
	}
	else
	{
		printf("Second SetNamedSecurityInfo call failed: %u\n",
			dwRes); 
	}

Cleanup:

	if (pSIDAdmin)
		FreeSid(pSIDAdmin); 

	if (pSIDEveryone)
		FreeSid(pSIDEveryone); 

	if (pACL)
		LocalFree(pACL);

	if (hToken)
		CloseHandle(hToken);

	return bRetval;
}


typedef void (WINAPI *LPFN_PGNSI)(LPSYSTEM_INFO);
typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
typedef BOOL (WINAPI *LPFN_DISABLEWOW64FSRED)(PVOID*);
typedef BOOL (WINAPI *LPFN_RESTOREWOW64FSRED)(PVOID);
static PVOID oldWow64 = NULL;

//WINBASEAPI BOOL WINAPI Wow64RevertWow64FsRedirection ( __in PVOID OlValue )
BOOL DisableWow64FsRedir()
{
	BOOL bRet = FALSE;
	PVOID* pOldWow64 = &oldWow64;

	LPFN_DISABLEWOW64FSRED fnDisable = (LPFN_DISABLEWOW64FSRED) GetProcAddress(GetModuleHandle("kernel32"),"Wow64DisableWow64FsRedirection");
	if (fnDisable){
		bRet = fnDisable(pOldWow64);
	}
	return bRet;
}

BOOL RestoreWow64FsRedir()
{
	BOOL bRet = FALSE;
	LPFN_RESTOREWOW64FSRED fnRestore = (LPFN_RESTOREWOW64FSRED) GetProcAddress(GetModuleHandle("kernel32"),"Wow64RevertWow64FsRedirection");
	if (fnRestore)
	{
		bRet = fnRestore(oldWow64);
	}
	return bRet;
}

//检查文件或目录是否存在
BOOL FileExists(LPCSTR lpszFileName, BOOL bIsDirCheck)
{
	//取得文件属性
	DWORD dwAttributes = ::GetFileAttributes(lpszFileName);
	if ( INVALID_FILE_ATTRIBUTES == dwAttributes)
	{
		return FALSE;
	}
	//是目录
	if (dwAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		if (bIsDirCheck) //当前检测的也是目录        
			return TRUE;        
		else
			return FALSE;
	}
	else //是文件
	{
		if (bIsDirCheck)       
			return FALSE;        
		else
			return TRUE;
	}
}


void AnalyseShortCut(const char *pFileName,
	OUT std::string& strRealPath,
	OUT std::string& strCommand,
	OUT std::string& strDescription
	)
{
	FILE			*file;
	unsigned short	usLenTemp;
	int				iSize;
	LNKHEAD			head;
	FILELOCATIONINFO	fileLocationInfo;
	char			szDescription[1024] = { 0 }; //快捷方式所指向的文件描述
	char			szFilePath[1024] = { 0 };    //快捷方式所指向的文件
	char			szCommand[1024] = { 0 };
	WCHAR			wszTemp[512] = { 0 };
	char*			pDest = NULL;
	DWORD			dwFlags;
	int				p;
	if ((fopen_s(&file, pFileName, "rb")) != 0 || !file)
	{
		return;
	}
	do
	{
		// head
		iSize = sizeof(LNKHEAD);
		if (fread(&head, 1, iSize, file) != iSize)
		{
			break;
		}
		dwFlags = head.dwFlags;
		if (dwFlags & LNK_HASIDLIST)
		{
			// The Shell Item Id List
			if (fread(&usLenTemp, 2, 1, file) != 1)
			{
				break;
			}
			//fread(&szCommand, usLenTemp, 1, file);
			fseek(file, usLenTemp, SEEK_CUR);
		}
		p = ftell(file);
		// file location info
		if (fread(&fileLocationInfo, sizeof(fileLocationInfo), 1, file) != 1)
		{
			break;
		}
		//fread(&szCommand, fileLocationInfo.dwSize - sizeof(fileLocationInfo), 1, file);
		fseek(file, fileLocationInfo.dwOffsetOfBasePath + p, SEEK_SET);
		if (fileLocationInfo.dwFlags & LNK_NETSHARE)
		{
			iSize = fileLocationInfo.dwOffsetOfNetworkVolume - fileLocationInfo.dwOffsetOfBasePath;
		}
		else
		{
			iSize = fileLocationInfo.dwOffsetOfRemainingPath - fileLocationInfo.dwOffsetOfBasePath;
		}
		if (fread(&szCommand, 1, iSize, file) != iSize)
		{
			break;
		}
		_strupr_s(szCommand,sizeof(szCommand)/sizeof(szCommand[0]));
		pDest = szCommand;
		if (NULL == strstr(pDest, ".EXE"))
		{
			strRealPath = pDest;
			break;
		}
		sprintf_s(szFilePath, "\"%s\"", szCommand);
		strRealPath = szFilePath;
		fseek(file, fileLocationInfo.dwSize + p, SEEK_SET);
		if (dwFlags & LNK_HASDES)
		{
			// skip Description string
			if (fread(&usLenTemp, 2, 1, file) != 1)
			{
				break;
			}
			if (fread(&wszTemp, sizeof(WCHAR), usLenTemp, file) != usLenTemp)
			{
				break;
			}
			wszTemp[usLenTemp] = '\0';
			WideCharToMultiByte(CP_ACP, 0, wszTemp, -1,
				szDescription, 512, NULL, NULL);
			//fseek(file, usLenTemp*2, SEEK_CUR);
		}
		if (dwFlags & LNK_HASPATH)
		{
			// skip Relative path
			if (fread(&usLenTemp, 2, 1, file) != 1)
			{
				break;
			}

			if (fread(&wszTemp, sizeof(WCHAR), usLenTemp, file) != usLenTemp)
			{
				break;
			}
			wszTemp[usLenTemp] = 0;
			WideCharToMultiByte(CP_ACP, 0, wszTemp, -1,
				szCommand, 512, NULL, NULL);
			//fseek(file, usLenTemp*2, SEEK_CUR);
		}
		if (dwFlags & LNK_HASWORKDIR)
		{
			// skip Working directory
			if (fread(&usLenTemp, 2, 1, file) != 1)
			{
				break;
			}
			if (fread(&wszTemp, sizeof(WCHAR), usLenTemp, file) != usLenTemp)
			{
				break;
			}
			wszTemp[usLenTemp] = '\0';
			WideCharToMultiByte(CP_ACP, 0, wszTemp, -1,
				szCommand, 512, NULL, NULL);
			//fseek(file, usLenTemp*2, SEEK_CUR);
		}
		if (dwFlags & LNK_HASCMD)
		{
			// Command line arguments
			if (fread(&usLenTemp, 2, 1, file) != 1)
			{
				break;
			}

			if (fread(&wszTemp, sizeof(WCHAR), usLenTemp, file) != usLenTemp)
			{
				break;
			}
			wszTemp[usLenTemp] = '\0';
			WideCharToMultiByte(CP_ACP, 0, wszTemp, -1,
				szCommand, 512, NULL, NULL);
			strcat_s(szFilePath,sizeof(szFilePath), " ");
			strcat_s(szFilePath, sizeof(szFilePath), szCommand);
			strCommand = szCommand;
		}
		_splitpath_s(pFileName, NULL, 0, NULL, 0, szDescription, sizeof(szDescription)/sizeof(szDescription[0]), NULL,0);
		strDescription = szDescription;
	} while (0);
	fclose(file);
}