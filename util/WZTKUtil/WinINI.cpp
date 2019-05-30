#include <windows.h>
#include "util/WinINI.h"

void CWinINI::DeleteSection(const wchar_t* szSection)
{
	WritePrivateProfileStringW(szSection, NULL, NULL, m_wstrIniFile.c_str());
}

void CWinINI::WriteString(const wchar_t* szKey, const wchar_t* szValue)
{
	if (m_wstrSection.empty())
	{
		return;
	}
	WritePrivateProfileStringW(m_wstrSection.c_str(), szKey, szValue, m_wstrIniFile.c_str());
}

std::wstring CWinINI::ReadString(const wchar_t* key)
{
	wchar_t buf[4096] = { 0 };
	GetPrivateProfileStringW(m_wstrSection.c_str(), key, NULL, buf, sizeof(buf), m_wstrIniFile.c_str());
	return std::wstring(buf);
}

void CWinINI::ReadInteger(const wchar_t* key, int& val)
{
	int defaultv = val;
	val = GetPrivateProfileIntW(m_wstrSection.c_str(), key, defaultv, m_wstrIniFile.c_str());
}
