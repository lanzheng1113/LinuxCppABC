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
    ofstream ofs;
    ofs.open("/home/duocore/configure/test.torrent");
    if (!ofs.is_open())
    {
        perror("open test torrent file");
    }
//    libtorrent::bdecode_node bn;
//    libtorrent::bdecode();
    return 0;
}

