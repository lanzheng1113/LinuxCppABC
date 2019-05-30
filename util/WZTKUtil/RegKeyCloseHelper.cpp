#include <windows.h>
#include "util/RegKeyCloseHelper.h"


CRegKeyCloseHelper::CRegKeyCloseHelper( HKEY hkey )
{
	_hkey = hkey;
}

CRegKeyCloseHelper::~CRegKeyCloseHelper()
{
	if (_hkey)
	{
		RegCloseKey(_hkey);
		_hkey = 0;
	}
}
