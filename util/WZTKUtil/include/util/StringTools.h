#pragma once
#include <vector>
#include <string>

#ifndef OUT
#define OUT
#endif

/**
 * \addtogroup STRINGS
 * \{
 */

/**
 * \brief 将一个std::vector<std::wstring>数组input转换为大小为cbBufferMaxLen的WCHAR数组outBuffer。
 * 使用NULL作为分隔符，并在最后一个位置额外加一个NULL
 *
 * \param outBuffer:存放转换后字符串数组的缓冲区
 * \param cbBufferMaxLen : outBuffer的大小。字节为单位，不是字符个数！
 * \param input : 需要转换的字符数组
 * \remark 
 * 这个函数一般和WCharArrayToStringVector配合用于在DLL和EXE之间传递字符串数组。
 */
void StringVectorToWCharArray( 
	OUT wchar_t* outBuffer,
	int cbBufferMaxLen,
	const std::vector<std::wstring>& input
	);

/**
 * \brief 将经过StringVectorToWCharArray处理的字符串重新转换回字符串数组 @see StringVectorToWCharArray 
 * \remark 
 * 这个函数一般和WCharArrayToStringVector配合用于在DLL和EXE之间传递字符串数组。
 */
std::vector<std::wstring> WCharArrayToStringVector( wchar_t* buf, int cbBuf );

/**
 * \brief 获取环境变量的值。
 */
std::wstring GetEnvVar( IN const std::wstring& wstrEnvironVar );

/**
 * \brief 拓展一个字符串，将其中的"%环境变量%"替换为本地路径。
 */
BOOL ExpandEnvVar(IN OUT std::wstring& wszPath);

/**
 * \}
 */