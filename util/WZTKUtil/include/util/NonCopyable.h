/*
 * QuteCom, a voice over Internet phone
 * Copyright (C) 2010 Mbdsys
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * \note : 这部分代码摘自QuteCom工程，上面为其License说明。文件中的代码已经移除部分跨平台的代码。
 * 
 */

#ifndef OWNONCOPYABLE_H
#define OWNONCOPYABLE_H

/**
 * \defgroup BASIC_CLASS 简单基础类
 * \brief 简单基础类,例如不可拷贝，纯虚接口类等
 * \{
 */
/**
 * Ensures derived classes have private copy constructor and copy assignment.
 *
 * Example:
 * <pre>
 * class MyClass : NonCopyable {
 * public:
 *    ...
 * };
 * </pre>
 *
 * Taken from Boost library.
 *
 * @see boost::noncopyable
 * @author Tanguy Krotoff
 */
class NonCopyable {
protected:

	 NonCopyable();

	 ~NonCopyable();

private:

	/**
	 * Copy constructor is private.
	 */
	NonCopyable(const NonCopyable &);

	/**
	 * Copy assignement is private.
	 */
	const NonCopyable & operator=(const NonCopyable &);
};

/**
 * \}
 */

#endif	//OWNONCOPYABLE_H

