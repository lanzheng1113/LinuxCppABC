/**
 * \copyright (C)2016 万兆天空
 * \file FileSignature.h
 * \brief 声明获取签名信息的接口。
 *
 * - v1.0 
 *   -# GetSignatureInfo 获取一个PE文件的数字签名信息
 *
 * \author FDD
 * \date 2017/8/15
 * \version 1.0
 */
#pragma once
#include <string>

#ifndef OUT
#define OUT
#endif

/**
 * \defgroup PE_SIGN 获取PE文件的数字签名信息
 * \brief 获取PE文件的数字签名信息
 * \{
 */

/**
 * \brief 签名信息
 */
struct SignerCertificate
{
	std::wstring IssuerName;	///< 证书颁发者
	std::wstring SubjectName;   ///< 签名者姓名，如：GuangZhou KuGou Computer Technology Co.,Ltd.
};

/**
 * \brief 获取一个PE文件的数字签名信息
 * \param path 文件路径
 * \param SignerInfo 签名信息
 * \return 成功返回0，否则返回其他值。
 */
int GetSignatureInfo(const std::wstring& path, OUT SignerCertificate &SignerInfo);

/**
 * \}
 */