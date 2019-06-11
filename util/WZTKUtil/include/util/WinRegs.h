#pragma once
#include <string>
#include <windows.h>
#include <string>
#include <map>
#include <vector>

using std::wstring;
using std::map;
using std::vector;

#ifndef OUT
#define OUT
#endif
/**
 * \addtogroup REG_FUN 
 * \{
 */

/**
 * \brief 获取HKEY_CURRENT_USER的正式路径.通过获取当前进程用户的SID，获取HKCU在HKUSER中的真实路径.
 * 例如“HKEY_USERS\S-1-5-21-1347714902-2505543936-4047459812-1000”
 * 
 * \return 返回HKEY_CURRENT_USER的路径，如果为空表示获取的过程中发生了错误，可以使用GetLastError()获取错误原因。
 * \note 此函数不可以用于服务进程，因为服务进程的用于是SYSTEM。
 * 对于服务进程来说没有登录用户的概念。
 */
std::string GetHKCURealPath();


/**
 * \brief 使当前进程获取注册表键的所有权以访问注册表键值.
 * \param hPrefKey 根键，例如HKEY_CURRENT_USER
 * \param lpczSubKey 需要获取所有权的注册表键所在的路径。
 * \return 成功返回0，否则返回一个错误ID（GetLastError）。
 */
DWORD TakeObjectsOwnerShip( HKEY hPrefKey, LPCSTR lpczSubKey );



/**
 * \brief 设置一个注册表的键值为指定的DWORD值
 * \param hKeyRoot HKLM/HKCU等根键
 * \param regPath 路径
 * \param keyName 键
 * \param val 值
 * \param WriteTo64bitRegPath 此路径是否为64位系统键值
 * \return None
 */
BOOL RegistrySetValueDowrd(HKEY hKeyRoot, const char* regPath, const char* keyName,DWORD val,BOOL WriteTo64bitRegPath);


/**
 * \brief 设置一个注册表的键值为指定的字符串值
 * \param hKeyRoot HKLM/HKCU等根键
 * \param regPath 路径
 * \param keyName 键
 * \param val 值
 * \param WriteTo64bitRegPath 此路径是否为64位系统键值
 * \return None
 */
BOOL RegistrySetValueString(HKEY hKeyRoot,const char* regPath, const char* keyName, const char* val,BOOL WriteTo64bitRegPath, bool isExpandStrType = false);

/**
 * \}
 */

class CWinRegKey
{
public:
	CWinRegKey(HKEY keyRoot, const wchar_t* subKey, BOOL bIs64) 
	{ 
		m_rootKey = keyRoot;
		m_subKey = subKey;
		m_bIs64 = bIs64;
	}
	~CWinRegKey() { ; }
public:
	/**
	 * \brief Set root key.
	 * \param keyRoot The root key.
	 */
	void SetRootKey(HKEY keyRoot) { m_rootKey = keyRoot; }
	/**
	 * \brief Set sub-key.
	 * \param subKey sub-key
	 */
	void SetSubKey(const wchar_t* subKey) { m_subKey = subKey; }
	/**
	 * \brief 设定是否为64位的注册表路径
	 */
	void SetIs64Entry(BOOL bIs64) { m_bIs64 = bIs64; }
	/**
	 * \brief Write an integer value to registry entry (with REG_DOWRD type).
	 * \param key The registry key.
	 * \param val The registry value.
	 */
	BOOL WriteInteger(const wchar_t* key, int val);

	/**
	 * \brief 读一个整形
	 * 
	 * \param
	 * \return
	 */
	BOOL ReadInteger(const wchar_t* key, OUT int* val);
	/**
	 * \brief Write an string value to registry entry (with REG_STRING type)
	 * \param key The registry key.
	 * \param val The registry value.
	 */
	BOOL WriteString(const wchar_t* key, const wchar_t* val, bool isExpandStrType=false);

	/**
	 * \brief 读一个字符串。
	 * 
	 * \param
	 * \return
	 */
	BOOL ReadString(const wchar_t* key, OUT wchar_t* val, int nSize);

	/**
	 * \brief 遍历当前子项下的所有值的键值对
	 * 
	 * \return 返回当前子项下的键值对
	 */
	std::map<std::wstring, std::wstring> EnumStringValues();

	/**
	 * \brief 遍历当前子项下的所有子项
	 * 
	 * \return 返回当前子项下的所有子项。
	 */
	std::vector<std::wstring> EnumSubKeys();

	/**
	 * \brief Create a sub-key under root key specified by "keyRoot" parameter.
	 */
	static BOOL CreateSubKey(HKEY keyRoot, const wchar_t* subKey);

	// delete sub key.
	BOOL DeleteSubKey();

	// 删除指定名字的值
	BOOL DeleteValue(const wchar_t* pszValueName);

	/**
	 * \brief Adjust a token's privilege.
	 * 
	 * \param hToken access token handle
	 * \param nameOfPrivilege name of privilege to enable/disable
	 * \param bEnablePrivilege to enable or disable privilege
	 * \return 成功返回TRUE，否则返回FALSE。调用GetLastError获取错误码。
	 */
	static BOOL SetPrivilege(HANDLE hToken, LPCWSTR nameOfPrivilege, BOOL bEnablePrivilege);
	/**
	 * \brief Load registry from a file.
	 * 
	 * \param KeyRoot
	 * \param lpSubKey
	 * \param lpFilePath
	 * \return
	 */
	static BOOL LoadKey(HKEY KeyRoot, LPCWSTR lpSubKey, LPCWSTR lpFilePath);

	/**
	 * \brief Unload Registry key.
	 */
	static BOOL UnLoadKey(HKEY KeyRoot, LPCWSTR lpSubKey);

private:
	BOOL DeleteSubKeyImp(HKEY hKey, LPWSTR subKey);

	CWinRegKey();
	CWinRegKey(const CWinRegKey&);
	CWinRegKey& operator=(const CWinRegKey& rh) {
		m_rootKey = rh.m_rootKey;
		m_subKey = rh.m_subKey;
		m_bIs64 = rh.m_bIs64;
		return *this; 
	}

private:
	HKEY m_rootKey;
	std::wstring m_subKey;
	BOOL m_bIs64;
};

