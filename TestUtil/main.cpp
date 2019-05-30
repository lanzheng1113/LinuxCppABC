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
using namespace std;

#include "util/md5.h"
#include "util/DateTime.h"

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
    return true;
}

#define TEST(FUN) (cout << #FUN << "\t" << (FUN()?"OK":"FAILED") << endl)

int main(int argc, char** argv) {
    TEST(test_md5);
    TEST(test_date_time);
    cout << "Press Enter key to continue..." << endl;
    fgetc(stdin);
    return 0;
}

