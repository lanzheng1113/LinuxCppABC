/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "EasyTimer.h"
#include <iostream>
using namespace std;

void test_timer() {
    EasyTimer e;
    e.SetTimer(1, 2000, [ = ]{
        static int i = 0;
        cout << "timer:1 tick:" << i++ << endl;
    });
    boost::this_thread::sleep(boost::posix_time::seconds(10));
    e.KillTimer(1);
    cout << "Enter any key to quit." << endl;
    char b;
    cin >> b;
}

