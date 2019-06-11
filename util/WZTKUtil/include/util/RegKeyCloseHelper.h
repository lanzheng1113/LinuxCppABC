/**
 * \file CommonTools.h 
 * \brief 此文件定义CRegKeyCloseHelper类
 * \version 1.0.0.1
 * \author duo
 * \date 20160823
 */

#pragma once

/**
 * @defgroup REG_FUN 注册表操作
 * @{
 */

/**
 * @class CRegKeyCloseHelper
 * \brief 这个类用于在离开作用域时关闭一个注册表键
 * 
 */
class CRegKeyCloseHelper
{
public:
	/**
	 * CRegKeyCloseHelper的构造函数，要求传入一个已经打开的HKEY。在离开作用域后将自动关闭它。
	 * \param hk 离开作用域时需要关闭的注册表键。
	 */
	CRegKeyCloseHelper(HKEY hkey);
	~CRegKeyCloseHelper();
protected:
private:
	HKEY _hkey;
};

/** @} */