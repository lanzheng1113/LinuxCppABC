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
#include "util/md5.h"
#include <stdio.h>
#include <string.h>
#include <iostream>
using namespace std;


/*
 * 
 */
int main(int argc, char** argv) {
    CMD5CheckSum ck;
    string szMD5 = ck.GetMD5OfString("12345");
    cout << szMD5 << endl;
    cout << szMD5.length() << endl;
    if ("827CCB0EEA8A706C4C34A16891F84E7B" == string(szMD5))
        cout << "Correct!" << endl;
    else
        cout << "Not correct!" << endl;
    printf("Press Enter key to continue..."); 
    fgetc(stdin);
    return 0;
}

