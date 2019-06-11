/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: duocore
 *
 * Created on 2019年6月4日, 下午9:10
 */

#include <cstdlib>
#include "bdecode.hpp"
#include <fstream>
#include <iostream>
#include <errno.h>
#include <cstring>
#include <sstream>
using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    /*
     * int bdecode(char const* start, char const* end, bdecode_node& ret
            , error_code& ec, int* error_pos = 0, int depth_limit = 100
            , int token_limit = 1000000);
     */
    ifstream ifs;
    ifs.open("/home/duocore/configure/test.torrent");
    if (!ifs.is_open()) {
        //printf("%d, %s\n",errno,strerror(errno));
        perror("open test torrent file");
        return errno;
    }

    stringstream ss;
    ss << ifs.rdbuf();
    ifs.close();
    string file_data = ss.str();
    cout << "length of torrent file:" << file_data.length() << endl;

    bdecode_node bn;
    error_code ec;
    int ret = bdecode(file_data.data(), file_data.data()+file_data.length(), bn, ec);
    if (ret)
    {
        cout << "bcdcode failed with error code: " << ec.value() << " - " << ec.message() << endl;
        return 0;
    }
    else
    {
        cout << print_entry(bn, false, 4) << endl;
    }
    return 0;
}

