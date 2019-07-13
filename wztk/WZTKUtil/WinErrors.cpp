#include "util/WinErrors.h"
#include <strsafe.h>

std::wstring ErrorMessageW(LPWSTR lpszFunction)
{ 
	// Retrieve the system error message for the last-error code
	std::wstring ret;
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError(); 

	FormatMessageW(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR) &lpMsgBuf,
		0, NULL );

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
		(wcslen((LPCWSTR)lpMsgBuf) + wcslen((LPCWSTR)lpszFunction) + 40) * sizeof(WCHAR)); 

	StringCchPrintfW((LPWSTR)lpDisplayBuf, 
		LocalSize(lpDisplayBuf) / sizeof(WCHAR),
		L"%s failed with error %d: %s", 
		lpszFunction, dw, lpMsgBuf); 

	//MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 
	ret = (LPCWSTR)lpDisplayBuf;

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);

	return ret;
}