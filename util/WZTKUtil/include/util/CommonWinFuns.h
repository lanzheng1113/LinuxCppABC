/**
 * \copyright (C)2016 万兆天空
 * \file CommonWinFuns.h  
 * \brief 定义常见的一些未归类的windows功能。
 * 
 * - v1.0
 *   -# GetAccountSid 根据一个用户名获取用户的sid。
 *   -# GetDefaultBrowserLaunchCommand 从注册表中获取默认浏览器的命令行。
 *   -# SetPrivilege 设置或者禁止一个令牌的权限。
 *   -# GetIEPath 获取IE路径。
 *   -# TakeOwnership 通过重设一个文件夹/文件的访问权限为EveryOne以获取访问权限。
 *   -# RestoreWow64FsRedir 恢复WOW64文件重定向。
 *   -# DisableWow64FsRedir 禁用WOW64文件重定向。
 *
 * - v1.1 新增获取文件版本信息和版权信息的接口。
 *   -# GetPEFileVersionInfo 获取PE文件中某个指定的资源段信息。
 *   -# GetFileLegalCopyright 获取PE文件中的版权信息。
 *   -# AnalyseShortCut 获取一个快捷方式的详细信息。
 * \author FDD
 * \date 2017/8/15
 * \version 1.1
 */
#pragma once
#include <string>
using std::wstring;


/**
 * \brief 根据一个用户名获取用户的sid.
 * \param[in] AccountName 输入的用户名.
 * \param[out] Sid 指向一个PSID的指针，用于接收返回的结果。在使用完Sid后，注意使用LocalFree释放它。
 * \return 成功返回true,并在Sid参数中返回用户名对应的SID,失败返回false，Sid被设置为NULL.
 * \note 在使用完Sid后，注意使用LocalFree释放它。
 */
bool GetAccountSid(LPSTR AccountName, OUT PSID *Sid);

/**
 * \brief 从注册表中获取默认浏览器的命令行.
 *  例如"C:\Program Files (x86)\Google\Chrome\Application\chrome.exe" -- "%1"
 * \param[out] default_browse 默认浏览器的命令行，仅仅当此函数返回true的时候有效。
 * \return 成功返回true,失败返回false.
 */
bool GetDefaultBrowserLaunchCommand(std::string& default_browser);


/**
 * \brief 设置或者禁止一个令牌的权限
 * 
 * \param hToken 已经打开的令牌句柄
 * \param lpszPrivilege 系统预定义的一些权限，例如SE_TAKE_OWNERSHIP_NAME。
 * \param bEnablePrivilege 如果设置为TRUE则设置令牌具有此权限，否则禁止令牌拥有此权限。
 * \return 成功返回TRUE，失败返回FALSE。
 */
BOOL SetPrivilege( HANDLE hToken, LPCSTR lpszPrivilege, BOOL bEnablePrivilege );


/**
 * \brief 获取IE路径
 * \return 成功返回IE的路径，失败返回空值。
 * \note:
 *	通过HKEY_CLASSES_ROOT\IE.HTTP\shell\open\command的默认值获取IE的路径，"C:\Program Files (x86)\Internet Explorer\iexplore.exe" -nohome
 *	通过字符串处理获取到最终的结果是C:\Program Files (x86)\Internet Explorer\iexplore.exe
 */
std::wstring GetIEPath();


/**
 * \brief 通过重设一个文件夹/文件的访问权限为EveryOne以获取访问权限。
 * \return 成功TRUE，失败返回FALSE。
 * \note:
 *	当文件夹的访问权限被设置为所有用户都无法访问时，即使当前用户拥有管理员权限也无法直接访问，
 *	但可以使用管理员权限先修改权限。这个函数实现了修改权限的功能。
 */
BOOL TakeOwnershipOfFilePath(LPSTR lpszOwnFile);


/**
 * \brief 恢复WOW64文件重定向
 */
BOOL RestoreWow64FsRedir();


/**
 * \brief 禁用WOW64文件重定向 
 */
BOOL DisableWow64FsRedir();


/**
 * \brief 获取PE文件中某个指定的资源段信息。
 * 可根据参数 strVersionKey 的值获取PE文件的版本版权信息等，见参数strVersionKey的说明。
 * \param strFilePath 文件路径
 * \param 希望获取的文件信息，如下：
 * - CompanyName
 * - FileDescription
 * - FileVersion
 * - InternalName
 * - LegalCopyright
 * - OriginalFilename
 * - ProductName
 * - ProductVersion
 * - Comments
 * - LegalTrademarks
 * - PrivateBuild
 * - SpecialBuild
 * \return 返回获取的资源信息。
 */
std::string GetPEFileVersionInfo(const std::string& strFilePath, const std::string& strVersionKey);


/**
 * \brief 获取PE文件中的版权信息。@see GetPEFileVersionInfo ，是它的一个特例。
 * \param strFilePath 文件路径
 * \return 返回文件的版权信息
 */
std::string GetFileLegalCopyright(CHAR * strFilePath);


typedef struct _LNKHEAD
{
	DWORD		dwID;
	DWORD		dwGUID[4];
	DWORD		dwFlags;
	DWORD		dwFileAttributes;
	FILETIME	dwCreationTime;
	FILETIME	dwModificationTime;
	FILETIME	dwLastaccessTime;
	DWORD		dwFileLen;
	DWORD		dwIconNum;
	DWORD		dwWinStyle;
	DWORD		dwHotkey;
	DWORD		dwReserved1;
	DWORD		dwReserved2;
}LNKHEAD, *PLNKHEAD;
typedef struct _FILELOCATIONINFO
{
	DWORD		dwSize;
	DWORD		dwFirstOffset;
	DWORD		dwFlags;
	DWORD		dwOffsetOfVolume;
	DWORD		dwOffsetOfBasePath;
	DWORD		dwOffsetOfNetworkVolume;
	DWORD		dwOffsetOfRemainingPath;
}FILELOCATIONINFO, *PFILELOCATIONINFO;
enum LinkKindEnum
{
	LNK_LOCALVOLUME = 0x1,
	LNK_NETSHARE = 0x2
};
enum LinkTypeEnum
{
	LNK_HASIDLIST = 0x1,
	LNK_FILEDIR = 0x2,
	LNK_HASDES = 0x4,
	LNK_HASPATH = 0x8,
	LNK_HASWORKDIR = 0x10,
	LNK_HASCMD = 0x20
};

/**
 * \brief AnalyseShortCut 解析快捷方式
 * \param pFileName 快捷方式(.lnk文件)自身的文件路径
 * \param strRealPath 执行后存放快捷方式所指向的文件路径
 * \param strCommand 执行后存放快捷方式的目标，包括命令行参数
 * \param strDescription 执行后存放快捷方式的描述
 */
void AnalyseShortCut(const char *pFileName, 
	OUT std::string& strRealPath, 
	OUT std::string& strCommand, 
	OUT std::string& strDescription );