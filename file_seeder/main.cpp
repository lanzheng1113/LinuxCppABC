/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 * file_seeder project is designed to run on a resource limited linux system with 1GB RAM, 1 core cpu and about a 30GB disk.
 * - The main perpor is running as a `torrent seeder`
 * - addtionally it can retrive the configure from a http server and update the current seed task.
 * So it also must have the ability to download files from the other client and web url.
 * - to update the tasks delete the old out-time files.
 */

/* 
 * File:   main.cpp
 * Author: duocore
 *
 * Created on 2019年7月2日, 下午9:23
 */

#include <cstdlib>
#include <iostream>
#include "task_status.h"
#include "async_call.h"
#include "sync_call.h"
#include "json/json.h"

using namespace std;

int main(int argc, char** argv) 
{
    cout << "hello world." << endl;
    return 0;
}

