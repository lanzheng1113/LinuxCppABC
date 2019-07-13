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
 */

#ifndef OWPATH_H
#define OWPATH_H

#include "util/global.h"

#include "util/NonCopyable.h"

#include <string>

namespace qcutil
{
/**
 * \brief Path class.
 *
 * Give following services:
 * - application path
 * - resources path
 * - config path
 *
 * Returned paths includes a trailing separator.
 *
 * \ingroup FILE_PATH
 */
class Path : NonCopyable {
public:

    /**
     * Gets the directory that contains the application executable.
     *
     * The executable path returned always finished by "/" or "\".
     *
     * On MacOS X, returns full path to exe. e.g: /Application/QuteCom.app/Contents/MacOS/QuteCom
     *
     * @return application executable path
     */
    static std::string getApplicationDirPath();


    /**
     * Gets the configuration directory, typically under user's
     * home directory.
     *
     * @return the configuration directory path
     */
    static std::string getConfigurationDirPath();


    /**
     * @see File::getPathSeparator()
     */
    static std::string getPathSeparator();


    /**
     * Gets the home directory path.
     *
     * @return the home dir of the current user.
     */
    static std::string getHomeDirPath();

    /**
     * \brief 移除尾部所有的"\",同shlwapi中的::PathRemoveBackslash
     */
    static void PathRemoveBackslash(std::string& str);
    static void PathRemoveBackslash(std::wstring& str);

    /**
     * \brief 移除尾部所有的"/"
     */
    static void PathRemoveSlash(std::string& str);
    static void PathRemoveSlash(std::wstring& str);
};
};

#endif //OWPATH_H
