/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: duocore
 *
 * Created on 2018年12月20日, 下午9:12
 */

#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

#include "util/md5.h"

bool test_md5() {
    CMD5CheckSum ck;
    string szMD5 = ck.GetMD5OfString("12345");
    //cout << szMD5 << endl;
    //cout << szMD5.length() << endl;
    if ("827CCB0EEA8A706C4C34A16891F84E7B" == string(szMD5)) {
        //cout << "Correct!" << endl;
        return true;
    } else {
        //cout << "Not correct!" << endl;
        return false;
    }
}

#include "util/DateTime.h"
#include "util/DateTimeSpan.h"

bool test_date_time() {
    DateTime dt(30, 5, 2019, 18, 34, 18);
    if (dt.toString() != "2019-05-30 18:34:18") return false;
    if (DateTime::daysOfMonth(2019, 5) != 31) return false;
    if (dt.calcWeekDay() != 4) return false;
    if (!dt.isValid()) return false;
    if (!(dt.getYear() == 2019 && dt.getMonth() == 5 && dt.getDay() == 30
            && dt.getHour() == 18 && dt.getMinute() == 34 && dt.getSecond() == 18)) {
        return false;
    }
    DateTime dt2(30, 5, 2019, 18, 34, 18);
    DateTime dt3(dt2);
    if (!(dt == dt2 && dt == dt3)) return false;

    DateTime dt4;
    dt4.setYear(2000);
    dt4.setMonth(2);
    dt4.setDay(14);
    dt4.setHour(23);
    dt4.setMinute(57);
    dt4.setSecond(4);
    DateTime dt5;
    DateTime::fromString("2000-02-14 23:57:04", dt5);
    if (!(dt4 == dt5)) return false;
    DateTime dt6;
    DateTime::fromStringSlash("2000/02/14/23/57/04", dt6);
    if (!(dt6 == dt5)) return false;
    if (DateTime::isValid(-1, 2, 4) || DateTime::isValid(2019, 2, 30)) return false;
    if (!DateTime::isLeapYear(2000) || DateTime::isLeapYear(2011)) return false;

    DateTime dt7, dt8;
    DateTime::fromString("2019-1-1 00:00:00", dt7);
    DateTime::fromString("2018-1-1 01:01:01", dt8);
    DateTimeSpan span = dt7 - dt8;
    //cout << span.GetDays() << "-" << span.GetHours() << "-" << span.GetMinutes() << "-" << span.GetSeconds() << endl;
    //cout << span.GetTotalHours() << "-" << span.GetTotalMinutes() << "-" << span.GetTotalSeconds() << endl;
    if (!(span.GetDays() == 364 && span.GetHours() == 22 && span.GetMinutes() == 58 && span.GetSeconds() == 59
            && span.GetTotalHours() == 8758 && span.GetTotalMinutes() == 525538 && span.GetTotalSeconds() == 31532339))
        return false;
    return true;
}

#include "util/Logger.h"

bool test_logger() {
    int i = 0;
    Logger::getInstance()->setLogFileName("TestLogger.txt");
    Logger::getInstance()->setIsPrintStdOutput(false);
    LOG_DEBUG("Debug message %d", ++i);
    LOG_INFO("Info message %d", ++i);
    LOG_WARN("Warnning message %d", ++i);
    LOG_ERROR("Error message %d", ++i);
    Logger::getInstance()->flush();

    ifstream ifs;
    ifs.open("TestLogger.txt");
    if (!ifs.is_open()) {
        cout << "failed to open logger file with error code" << errno << endl;
    }
    stringstream ss;
    ss << ifs.rdbuf();
    string file_content = ss.str();
    ifs.close();
    return (file_content.find("Debug message 1") != string::npos
            && file_content.find("Info message 2") != string::npos
            && file_content.find("Warnning message 3") != string::npos
            && file_content.find("Error message 4") != string::npos);
}

#include "util/crypt.h"

bool test_xor_crypt() {
    XorCrypter x(string(""));
    char msg[32] = "123456";
    char msg2[32] = "";
    char msg3[32] = "";
    x.SimpleXor_Crype(msg2, msg, sizeof (msg));
    x.SimpleXor_Crype(msg3, msg2, sizeof (msg3));
    //cout << msg3 << endl;
    return 0 == strcmp(msg, msg3);
}

#include "util/Des.h"

#ifndef _countof
#define _countof(ARRAY) (sizeof(ARRAY)/sizeof(ARRAY[0]))
#endif

bool test_des() {
    {
        //
        // Test CDes::EncryStrHex and CDes::DecryStrHex with string type paramters.
        //
        Util::CDes d;
        string strs = "123123123";
        string strd = "";
        string key = "This is a long long long long long key.";
        d.EncryStrHex(strs, strd, key);
        string strd2 = "";
        d.DecryStrHex(strd, strd2, key);
        //cout << "orignal string: " << strs << "\nencrypted: " << strd << "\ndecrypted:" << strd2 << endl;
        if (strs != strd2) {
            return false;
        }
    }
    {
        //
        // Test CDes::EncryStrHex and CDes::DecryStrHex with `char*` type paramters.
        // void EncryStrHex(const char* chrInText, char* chrOutText, const char* chrKeyText);
        //
        Util::CDes d;
        const char* pTexts = "123123123";
        char out[128] = {0};
        char out2[128] = {0};
        const char*  key = "This is a long long long long long key.";
        d.EncryStrHex(pTexts, out, key);
        d.DecryStrHex(out, out2, key);
        //cout << "orignal string: " << pTexts << "\nencrypted: " << out << "\ndecrypted:" << out2 << endl;
        if(strcmp(pTexts, out2))
            return false;
    }
    
    {
        //
        // Test EncryStrHex with `unsigned char*` type parameters.
        // void EncryStrHex(unsigned char *pchr_in, unsigned long llen, string &str_out, string str_key);
        // void DecryStrHex(string str_in, unsigned char **pchr_out, unsigned long &llen, string str_key);
        //
        Util::CDes d;
        unsigned char pSrc[4] = {0,1,2,3};
 
        string key = "This is a long long long long long key.";
        string out = "";
        d.EncryStrHex(pSrc, 4, out, key);
        //cout << out << endl;
        unsigned long llen = 0;
        unsigned char* px = NULL;
        d.DecryStrHex(out, &px, llen, key);
        //cout << "length = " << llen << endl;
        //for (int i=0; i!= llen; i++)
        //    cout << (int)px[i] << " ";
        //cout << endl;
        if (!(px && llen == 4 && *(int32_t*)px == 0x03020100))
            return false;
        if(px)
            delete []px;
    }

    return true;
}

#include "util/StringList.h"
bool test_string_list()
{
    StringList sl;
    sl += "1";
    sl += "2";
    if(!(sl.size() == 2) && "1" == sl[0] && "2" == sl[1]) return false;
    StringList sl2;
    sl2.push_back("3a");
    sl2 += "4B";
    sl += sl2;
    if(!(sl.size() == 4) && "1" == sl[0] && "2" == sl[1] && "3a" == sl[2] && "4B" == sl[3]) return false;
    if(!sl.contains("3a", true)) return false;
    if(!sl.contains("3A", false)) return false;
    StringList sl3(sl);
    string cc = sl3.join(" ");
    if("1 2 3a 4B" != cc) return false;
    sl3.sort(StringList::Ascendant);
    cc = sl3.join(" ");
    if("1 2 3a 4B" != cc) return false;
    sl3.sort(StringList::Descendant);
    cc = sl3.join(" ");
    if("4B 3a 2 1" != cc) return false;
    sl3.push_back("1");
    sl3 += "2";
    sl3 += "3a";
    sl3 += "4B";
    sl3.removeDuplicatedStrings();
    cc = sl3.join(" ");
    if("1 2 3a 4B" != cc) return false;
    
    return true;
}

#include "util/Path.h"
bool test_path()
{
//    cout << "getApplicationDirPath: "  << Path::getApplicationDirPath() << endl;
//    cout << "getConfigurationDirPath: " << Path::getConfigurationDirPath() << endl;
//    cout << "getPathSeparator:" << Path::getPathSeparator() << endl;
//    cout << "getHomeDirPath:" << Path::getHomeDirPath() << endl;
//    string cc = Path::getHomeDirPath();
//    Path::PathRemoveSlash(cc);
//    cout << cc << endl;
//    Path::PathRemoveSlash(cc);
//    cout << cc << endl;
    return true;
}


#define TEST(FUN) (cout << #FUN << "\t" << (FUN()?"OK":"FAILED") << endl)

int main(int argc, char** argv) {
    TEST(test_md5);
    TEST(test_date_time);
    TEST(test_logger);
    TEST(test_xor_crypt);
    TEST(test_des);
    TEST(test_string_list);
    TEST(test_path);
    cout << "Press Enter key to continue..." << endl;
    fgetc(stdin);
    return 0;
}

