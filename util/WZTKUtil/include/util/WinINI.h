#pragma once

#include <string>
using std::wstring;

class CWinINI
{
public:
	void SetIniFile(const wchar_t* szIniFile) { m_wstrIniFile = szIniFile; }
	void SetSectionName(const wchar_t* szSection) { m_wstrSection = szSection; }
	void DeleteSection(const wchar_t* szSection);
	void WriteString( const wchar_t* szKey, const wchar_t* szValue);
	/**
	 * \brief 读字符串，最多支持4095个字符。
	 * 
	 * \param key INI的键
	 * \return 对应字符串值
	 */
	std::wstring ReadString( const wchar_t* key );

	/**
	 * \brief 读取INI中的整形值
	 * 
	 * \param key INI的键
	 * \param [in][out] val 输入时作为默认值，作为输出变量时存放读取结果。
	 */
	void ReadInteger(const wchar_t* key,int& val);
protected:
private:
	std::wstring m_wstrIniFile;
	std::wstring m_wstrSection;
};