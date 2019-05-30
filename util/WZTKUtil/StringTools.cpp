#include <Windows.h>
#include <string>
#include <vector>
#include "util/StringTools.h"
#include "util/Path.h"

void StringVectorToWCharArray( OUT wchar_t* outBuffer,int cbBufferMaxLen,const std::vector<std::wstring>& input)
{
	if (cbBufferMaxLen < 2)
	{
		return;
	}
	if (input.empty())
	{
		outBuffer[0] = 0;
		outBuffer[1] = 0;
		return;
	}
	int index = 0;
	int iStringLen = 0;
	bool isFirstOne = true;
	for (int i=0; i!=input.size(); i++)
	{
		std::wstring m_strInstalledBrName = input[i];
		if (m_strInstalledBrName.empty())
		{
			continue;
		}
		if (!isFirstOne)
		{
			//第一个之前不加分隔符
			outBuffer[index] = 0;
			index ++;
		}
		else
		{
			isFirstOne = false;
		}

		iStringLen = m_strInstalledBrName.length();
		if (cbBufferMaxLen < (index+iStringLen+1)*sizeof(wchar_t))
		{
			//log("输入缓冲区太小。");
			return;
		}
		memcpy(outBuffer+index,m_strInstalledBrName.c_str(),iStringLen*sizeof(wchar_t));
		index += iStringLen;
	}
	//在最后一个位置再额外加一个0
	outBuffer[index] = 0;
	//log("成功组织输出缓冲。");
	return;
}


std::vector<std::wstring> WCharArrayToStringVector(wchar_t* buf,int cbBuf)
{
	std::vector<std::wstring> ret;
	if (cbBuf == 0)
	{
		return ret;
	}
	if(buf[0]=='\0')
		return ret;       //如果第一个字符就是0，则说明ini中一个节也没有

	wchar_t *pBrowserName;	//保存找到的某个节名字符串的首地址
	int i;					//i指向数组buf的某个位置，从0开始，顺序后移
	int j=0;				//j用来保存下一个节名字符串的首地址相对于当前i的位置偏移量
	int count=0;			//统计节的个数

	for(i=0; i<cbBuf-1; i++,j++)
	{
		if( buf[i] == 0 )
		{
			pBrowserName = &buf[i-j]; //找到一个0，则说明从这个字符往前，减掉j个偏移量，就是一个浏览器名的首地址
			j = -1; //找到一个浏览器名后，j的值要还原，以统计下一个浏览器名地址的偏移量赋成-1是因为浏览器名字符串的最后一个字符0是终止符，
			ret.push_back(pBrowserName);//把读出来的值加入vec
			if( buf[i+1]==0 )
			{
				break;      //当两个相邻的字符都是0时，则所有的浏览器名都已找到，循环终止
			} 
		}			
	}   
	return ret;
}

BOOL ExpandEnvVar( IN OUT std::wstring& wszPath )
{
	std::wstring::size_type pos1, pos2;
	std::wstring wszVar, wszDir;
	int len;

	pos1 = pos2 = 0;
	do 
	{
		pos1 = wszPath.find_first_of(TEXT('%'), pos2);
		if (pos1 == std::wstring::npos){
			break;
		}

		pos2 = wszPath.find_first_of(TEXT('%'), pos1 + 1);
		if (pos2 == std::wstring::npos){
			break;
		}

		len = pos2 - pos1 - 1;
		wszVar = wszPath.substr(pos1+1, len);
		wszDir = GetEnvVar(wszVar.c_str());    //用全局变量表展开变量
		if (!wszDir.empty())
		{
			Path::PathRemoveBackslash(wszDir);
			len = pos2 - pos1 + 1;
			wszPath.replace(pos1, len, wszDir);
		}

	} while (1);

	return TRUE;
}

std::wstring GetEnvVar( IN const std::wstring& wstrEnvironVar )
{
	wchar_t wszPath[MAX_PATH + 1];
	memset(wszPath, 0, sizeof(wszPath));
	GetEnvironmentVariableW(wstrEnvironVar.c_str(), wszPath, MAX_PATH);
	return wszPath;
}

