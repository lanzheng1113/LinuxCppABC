#include "util/WinRegs.h"
#include "util/StringEx.h"
#include "util/Logger.h"

BOOL CWinRegKey::WriteInteger(const wchar_t* key, int val)
{
	return RegistrySetValueDowrd(m_rootKey, String::fromStdWString(m_subKey).c_str(), String::fromStdWString(key).c_str(), val ,m_bIs64);
}

BOOL CWinRegKey::WriteString(const wchar_t* key, const wchar_t* val, bool isExpandStrType)
{
	return RegistrySetValueString(m_rootKey, 
		String::fromStdWString(m_subKey).c_str(), 
		String::fromStdWString(key).c_str(), 
		String::fromStdWString(val).c_str(), 
		m_bIs64, isExpandStrType);
}

BOOL CWinRegKey::ReadInteger(const wchar_t* key, OUT int* val)
{
	BOOL bRet = FALSE;
	HKEY hSubKey = 0;
	DWORD dw = sizeof(DWORD);
	DWORD valType = REG_DWORD;
	DWORD keyMask = m_bIs64 ? KEY_WOW64_64KEY : 0;
	if (ERROR_SUCCESS == RegOpenKeyExW(m_rootKey,m_subKey.c_str(),0,KEY_READ | keyMask, &hSubKey))
	{
		if (ERROR_SUCCESS == RegQueryValueExW(hSubKey,key,NULL,&valType,(LPBYTE)(val),&dw))
		{
			bRet = TRUE;
		}
		RegCloseKey(hSubKey);
	}
	return bRet;
}


BOOL CWinRegKey::ReadString(const wchar_t* key, OUT wchar_t* val, int nSize)
{
	BOOL bRet = FALSE;
	HKEY hSubKey = 0;
	DWORD dw = nSize;
	DWORD valType = REG_SZ;
	DWORD keyMask = m_bIs64 ? KEY_WOW64_64KEY : 0;
	if (ERROR_SUCCESS == RegOpenKeyExW(m_rootKey, m_subKey.c_str(), 0, KEY_READ | keyMask, &hSubKey))
	{
		if (ERROR_SUCCESS == RegQueryValueExW(hSubKey, key, NULL, &valType, (LPBYTE)val, &dw))
		{
			bRet = TRUE;
		}
		RegCloseKey(hSubKey);
	}
	return bRet;
}

BOOL CWinRegKey::CreateSubKey(HKEY keyRoot, const wchar_t* subKey)
{
	LSTATUS stat = ERROR_SUCCESS;
	DWORD dwRes;
	HKEY hKeyResult = (HKEY)0;

	//打开根目录
	stat = RegCreateKeyExW(keyRoot, subKey,
		0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKeyResult, &dwRes);
	if (stat == ERROR_SUCCESS) {
		RegCloseKey(hKeyResult);
		return TRUE;
	}
	else {
		return FALSE;
	}
}

BOOL CWinRegKey::DeleteSubKey()
{
	WCHAR KeyPath[1024] = { 0 };
	wcscpy_s(KeyPath, sizeof(KeyPath) / sizeof(KeyPath[0]), m_subKey.c_str());
	return DeleteSubKeyImp(m_rootKey, KeyPath);
}

typedef LONG(WINAPI * RegDeleteKeyExPtr)(HKEY, LPCWSTR, REGSAM, DWORD);
LONG WINAPI MyRegDeleteKeyEx(HKEY hKey, LPCWSTR subKey, REGSAM sam)
{
	LONG retval = 0;
	HMODULE hModule = LoadLibrary("ADVAPI32.dll");
	if (!hModule)
	{
		return ERROR_INCORRECT_SIZE;
	}
	RegDeleteKeyExPtr RDKE = (RegDeleteKeyExPtr)GetProcAddress(hModule, "RegDeleteKeyExW");
	if (RDKE)
		retval = RDKE(hKey, subKey, sam, 0);
	else
		retval = RegDeleteKeyW(hKey, subKey);
	FreeLibrary(hModule);
	return retval;
}


BOOL CWinRegKey::DeleteValue(const wchar_t* pszValueName)
{
	if (!pszValueName)
	{
		return FALSE;
	}
	DWORD keyMask = m_bIs64 ? KEY_WOW64_64KEY : 0;
	HKEY hKey = (HKEY)0;
	LONG lResult = RegOpenKeyExW(m_rootKey, m_subKey.c_str(), 0, KEY_WRITE | KEY_READ | keyMask, &hKey);
	if (lResult == ERROR_SUCCESS)
	{
		lResult = RegDeleteValueW(hKey,pszValueName);
		RegCloseKey(hKey);
		return lResult == ERROR_SUCCESS;
	}
	return FALSE;
}


BOOL CWinRegKey::DeleteSubKeyImp(HKEY hKeyRoot, LPWSTR lpSubKey)
{
	LPWSTR lpEnd;
	LONG lResult;
	DWORD dwSize;
	WCHAR szName[MAX_PATH];
	HKEY hKey;
	FILETIME ftWrite;
	DWORD keyMask = m_bIs64 ? KEY_WOW64_64KEY : 0;

	// First, see if we can delete the key without having to recurse.
	lResult = MyRegDeleteKeyEx(hKeyRoot, lpSubKey, keyMask);
	if (lResult == ERROR_SUCCESS)
		return TRUE;
	lResult = RegOpenKeyExW(hKeyRoot, lpSubKey, 0, KEY_READ| KEY_WRITE | keyMask, &hKey);
	if (lResult != ERROR_SUCCESS)
	{
		if (lResult == ERROR_FILE_NOT_FOUND) {
			printf("Key not found.\n");
			return TRUE;
		}
		else {
			printf("Error opening key.\n");
			return FALSE;
		}
	}
	// Check for an ending slash and add one if it is missing.
	lpEnd = lpSubKey + lstrlenW(lpSubKey);
	if (*(lpEnd - 1) != L'\\')
	{
		*lpEnd = L'\\';
		lpEnd++;
		*lpEnd = L'\0';
	}
	// Enumerate the keys
	dwSize = MAX_PATH;
	lResult = RegEnumKeyExW(hKey, 0, szName, &dwSize, NULL, NULL, NULL, &ftWrite);
	if (lResult == ERROR_SUCCESS)
	{
		do {
			lstrcpyW(lpEnd, szName);
			if (!DeleteSubKeyImp(hKeyRoot, lpSubKey))
			{
				break;
			}
			dwSize = MAX_PATH;
			lResult = RegEnumKeyExW(hKey, 0, szName, &dwSize, NULL,NULL, NULL, &ftWrite);
		} while (lResult == ERROR_SUCCESS);
	}

	lpEnd--;
	*lpEnd = L'\0';
	RegCloseKey(hKey);

	// Try again to delete the key.
	lResult = MyRegDeleteKeyEx(hKeyRoot, lpSubKey, keyMask);
	if (lResult == ERROR_SUCCESS)
		return TRUE;
	return FALSE;
}

BOOL CWinRegKey::SetPrivilege(HANDLE hToken, LPCWSTR nameOfPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
	if (!LookupPrivilegeValueW(
		NULL,               // lookup privilege on local system
		nameOfPrivilege,   // privilege to lookup 
		&luid))           // receives LUID of privilege
	{
		LOG_ERROR("LookupPrivilegeValue error: %u\n", GetLastError());
		return FALSE;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	if (!AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL,
		(PDWORD)NULL))
	{
		LOG_ERROR("AdjustTokenPrivileges error: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)

	{
		LOG_ERROR("The token does not have the specified privilege. \n");
		return FALSE;
	}

	return TRUE;
}


BOOL CWinRegKey::LoadKey(HKEY KeyRoot, LPCWSTR lpSubKey, LPCWSTR lpFilePath)
{
	// 挂载C盘下的注册表
	HANDLE proccessHandle = GetCurrentProcess();     // get the handle to the current proccess
	DWORD typeOfAccess = TOKEN_ADJUST_PRIVILEGES;   //  requiered to enable or disable the privilege
	HANDLE tokenHandle;                             //  handle to the opened access token
	if (OpenProcessToken(proccessHandle, typeOfAccess, &tokenHandle))
	{
		// Enabling RESTORE and BACKUP privileges
		SetPrivilege(tokenHandle, String(SE_RESTORE_NAME).toStdWString().c_str(), TRUE);
		SetPrivilege(tokenHandle, String(SE_BACKUP_NAME).toStdWString().c_str(), TRUE);
	}
	else
	{
		LOG_ERROR("Error getting the access token with error %d.\n", GetLastError());
		return FALSE;
	}
	// 挂载文件如C:\windows\system32\config\SOFTWARE到指定的位置。
	HKEY hKey = KeyRoot;
	LPCWSTR subKeyName = lpSubKey;
	LPCWSTR pHive = lpFilePath;
	LONG loadKey = RegLoadKeyW(hKey, subKeyName, pHive);
	if (loadKey != ERROR_SUCCESS)
	{
		LOG_ERROR("Error loading the key. Code: %li\n", loadKey);
		return FALSE;
	}
	return TRUE;
}

BOOL CWinRegKey::UnLoadKey(HKEY KeyRoot, LPCWSTR lpSubKey)
{
	return ERROR_SUCCESS == RegUnLoadKeyW(KeyRoot, lpSubKey);
}

std::map<std::wstring, std::wstring> CWinRegKey::EnumStringValues()
{
	std::map<std::wstring, std::wstring> mapRet;
	DWORD keyMask = m_bIs64 ? KEY_WOW64_64KEY : 0;
	HKEY hKey = 0;
	if (RegOpenKeyExW(m_rootKey, m_subKey.c_str(), 0, KEY_QUERY_VALUE|KEY_READ|keyMask, &hKey) == ERROR_SUCCESS)
	{
		const int MAX_KEY_LENGTH = 255;
		const int MAX_VALUE_NAME = 16383;

		WCHAR    achClass[MAX_PATH] = L"";  // buffer for class name 
		DWORD    cchClassName = MAX_PATH;  // size of class string 
		DWORD    cSubKeys = 0;               // number of subkeys 
		DWORD    cbMaxSubKey;              // longest subkey size 
		DWORD    cchMaxClass;              // longest class string 
		DWORD    cValues;              // number of values for key 
		DWORD    cchMaxValue;          // longest value name 
		DWORD    cbMaxValueData;       // longest value data 
		DWORD    cbSecurityDescriptor; // size of security descriptor 
		FILETIME ftLastWriteTime;      // last write time 

		DWORD i, retCode;

		WCHAR  achValue[MAX_VALUE_NAME];
		DWORD cchValue = MAX_VALUE_NAME;

		// Get the class name and the value count. 
		retCode = RegQueryInfoKeyW(
			hKey,                    // key handle 
			achClass,                // buffer for class name 
			&cchClassName,           // size of class string 
			NULL,                    // reserved 
			&cSubKeys,               // number of subkeys 
			&cbMaxSubKey,            // longest subkey size 
			&cchMaxClass,            // longest class string 
			&cValues,                // number of values for this key 
			&cchMaxValue,            // longest value name 
			&cbMaxValueData,         // longest value data 
			&cbSecurityDescriptor,   // security descriptor 
			&ftLastWriteTime);       // last write time 

									 // Enumerate the key values. 

		if (cValues)
		{
			printf("Number of values: %d\n", cValues);
			for (i = 0, retCode = ERROR_SUCCESS; i < cValues; i++)
			{
				cchValue = MAX_VALUE_NAME;
				achValue[0] = '\0';
				DWORD dwType = 0;
				BYTE dataOfValue[1024] = { 0 };
				DWORD cbData = sizeof(dataOfValue);
				retCode = RegEnumValueW(hKey, i,
					achValue,
					&cchValue,
					NULL,
					&dwType,
					dataOfValue,
					&cbData);

				if (retCode == ERROR_SUCCESS && (dwType == REG_SZ || dwType == REG_EXPAND_SZ))
				{
					mapRet[achValue] = (const wchar_t*)dataOfValue;
				}
			}
		}
		RegCloseKey(hKey);
	}
	return mapRet;
}

std::vector<std::wstring> CWinRegKey::EnumSubKeys()
{
	std::vector<std::wstring> vecRet;
	HKEY hKey;
	DWORD keyMask = m_bIs64 ? KEY_WOW64_64KEY : 0;
	if (RegOpenKeyExW(m_rootKey, m_subKey.c_str(), 0, KEY_QUERY_VALUE|KEY_READ| keyMask, &hKey) == ERROR_SUCCESS)
	{
		const int MAX_KEY_LENGTH = 255;
		const int MAX_VALUE_NAME = 16383;

		WCHAR    achKey[MAX_KEY_LENGTH];   // buffer for subkey name
		DWORD    cbName;                   // size of name string 
		WCHAR    achClass[MAX_PATH] = L"";  // buffer for class name 
		DWORD    cchClassName = MAX_PATH;  // size of class string 
		DWORD    cSubKeys = 0;               // number of subkeys 
		DWORD    cbMaxSubKey;              // longest subkey size 
		DWORD    cchMaxClass;              // longest class string 
		DWORD    cValues;              // number of values for key 
		DWORD    cchMaxValue;          // longest value name 
		DWORD    cbMaxValueData;       // longest value data 
		DWORD    cbSecurityDescriptor; // size of security descriptor 
		FILETIME ftLastWriteTime;      // last write time 

		DWORD i, retCode;

		//TCHAR  achValue[MAX_VALUE_NAME]; 
		DWORD cchValue = MAX_VALUE_NAME;

		// Get the class name and the value count. 
		retCode = RegQueryInfoKeyW(
			hKey,                    // key handle 
			achClass,                // buffer for class name 
			&cchClassName,           // size of class string 
			NULL,                    // reserved 
			&cSubKeys,               // number of subkeys 
			&cbMaxSubKey,            // longest subkey size 
			&cchMaxClass,            // longest class string 
			&cValues,                // number of values for this key 
			&cchMaxValue,            // longest value name 
			&cbMaxValueData,         // longest value data 
			&cbSecurityDescriptor,   // security descriptor 
			&ftLastWriteTime);       // last write time 

									 // Enumerate the subkeys, until RegEnumKeyEx fails.

		if (cSubKeys)
		{
			printf("Number of sub-key(s): %d\n", cSubKeys);
			for (i = 0; i < cSubKeys; i++)
			{
				cbName = MAX_KEY_LENGTH;
				retCode = RegEnumKeyExW(hKey, i,
					achKey,
					&cbName,
					NULL,
					NULL,
					NULL,
					&ftLastWriteTime);
				if (retCode == ERROR_SUCCESS)
				{
					//_tprintf(TEXT("(%d) %s\n"), i+1, achKey);
					vecRet.push_back(achKey);
				}
			}
		}

		RegCloseKey(hKey);
	}
	return vecRet;
}
