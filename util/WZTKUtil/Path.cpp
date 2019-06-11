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
 */

#include "util/Path.h"
#include "util/File.h"
#include "util/StringEx.h"
#include "util/Logger.h"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

using namespace std;

static const int MAX_PATH = 256;

string Path::getApplicationDirPath() {
    string result;
    char procname[MAX_PATH];
    memset(procname, 0, sizeof (procname));
    pid_t pid = getpid();
    if (snprintf(procname, sizeof (procname), "/proc/%i/exe", pid) < 0) {
        return String::null;
    }

    char buff[MAX_PATH];
    memset(buff, 0, sizeof (buff));
    int ret = readlink(procname, buff, sizeof (buff));

    if (ret == -1 || (unsigned int) ret >= sizeof (buff)) {
        return String::null;
    }

    buff[ret] = 0;
    string mstr(buff);
    File f(mstr);
    result = f.getPath() + File::getPathSeparator();
    return result;
}

string Path::getConfigurationDirPath() {
    string result;

    char *appData = getenv("APPDATA");
    if (appData) {
        // remove the last separator to appData
        String tmpAppData = String(appData);
        if ((!tmpAppData.endsWith("\\")) || (!tmpAppData.endsWith("/"))) {
            tmpAppData = tmpAppData + File::getPathSeparator();
        }
        ////

        result = tmpAppData;
    } else {
        result = getHomeDirPath() + File::convertPathSeparators("Application Data/");
    }

    return result;
}

std::string Path::getPathSeparator() {
    return File::getPathSeparator();
}

string Path::getHomeDirPath() {
    string result;

    char * homeDrive = getenv("HOMEDRIVE");
    char * homeDir = getenv("HOMEPATH");
    if (homeDrive && homeDir) {

        // remove the last separator to homeDrive
        String tmpHomeDrive = String(homeDrive);
        if ((tmpHomeDrive.endsWith("\\")) || (tmpHomeDrive.endsWith("/"))) {
            tmpHomeDrive.substr(0, tmpHomeDrive.size() - 2);
        }
        ////

        // add the leading separator to homeDir
        String tmpHomeDir = String(homeDir);
        if ((!tmpHomeDir.beginsWith("\\")) || (!tmpHomeDir.beginsWith("/"))) {
            tmpHomeDir = File::getPathSeparator() + tmpHomeDir;
        }
        ////

        result = tmpHomeDrive + tmpHomeDir;
    }

    result += File::getPathSeparator();

    return result;
}

void Path::PathRemoveBackslash(std::string& str) {
    std::string::reverse_iterator p;
    for (p = str.rbegin(); p != str.rend(); ++p) {
        if (*p != '\\') {
            break;
        }
    }
    str.erase(p.base(), str.end());
}

void Path::PathRemoveBackslash(std::wstring& str) {
    std::wstring::reverse_iterator p;
    for (p = str.rbegin(); p != str.rend(); ++p) {
        if (*p != L'\\') {
            break;
        }
    }
    str.erase(p.base(), str.end());
}

void Path::PathRemoveSlash(std::string& str) {
    std::string::reverse_iterator p;
    for (p = str.rbegin(); p != str.rend(); ++p) {
        if (*p != '/') {
            break;
        }
    }
    str.erase(p.base(), str.end());
}

void Path::PathRemoveSlash(std::wstring& str) {
    std::wstring::reverse_iterator p;
    for (p = str.rbegin(); p != str.rend(); ++p) {
        if (*p != L'/') {
            break;
        }
    }
    str.erase(p.base(), str.end());
}
