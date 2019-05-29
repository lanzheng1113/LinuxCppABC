/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: duocore
 *
 * Created on 2019年5月28日, 上午11:41
 */

#include <cstdlib>
#include <iostream>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/mutex.hpp>
#include <deque>
using namespace std;

/**
 * Test sync-call in two thread.
 */

class TestSyncCallClass : public boost::enable_shared_from_this<TestSyncCallClass> {
public:
    typedef boost::function<void() > sync_call_type;

    TestSyncCallClass() {
        m_thread = NULL;
        m_thread_quit = false;
        m_thread_is_running = false;
        cout << __FUNCTION__ << endl;
    }

    ~TestSyncCallClass() {
        cout << __FUNCTION__ << endl;
    }

    void start() {
        if (m_thread)
            return;
        m_thread = new boost::thread(&TestSyncCallClass::thread_function, this, 1234);
    }

    sync_call_type get_from_local() {
        sync_call_type c;
        boost::recursive_mutex::scoped_lock a(m_mutex_task_array);
        if (!m_task_array.empty()) {
            // pop first task in task array.
            c = m_task_array.front();
            m_task_array.pop_front();
            cout << "Start a new task from array." << endl;
        }
        return c;
    }

    void thread_function(int nid) {
        cout << "thread start : id = " << nid << endl;
        m_thread_is_running = true;
        while (!m_thread_quit) {
            sync_call_type c = get_from_local();
            if (c) {
                c();
            } else
                boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        }
    }

    void sync_call_wait(bool& done, boost::condition_variable& cv, boost::mutex& m) {
        boost::mutex::scoped_lock a(m);
        while (!done) {
            cout << "Waiting for task finish." << endl;
            cv.wait(a);
        }
    }

    void sync_call_proxy(sync_call_type& fun, bool& done, boost::condition_variable& cv, boost::mutex& m) {
        fun();
        done = true;
        boost::mutex::scoped_lock a(m);
        cv.notify_all();
    }

    void add_task(sync_call_type& f) {
        boost::recursive_mutex::scoped_lock a(m_mutex_task_array);
        cout << "try add a task." << endl;
        m_task_array.push_back(f);
    }

    void sync_call(sync_call_type& fun) {
        bool done = false;
        boost::condition_variable cv;
        boost::mutex m;
        sync_call_type f = boost::bind(&TestSyncCallClass::sync_call_proxy, this,
                boost::ref(fun),
                boost::ref(done),
                boost::ref(cv),
                boost::ref(m));
        add_task(f);
        cout << "Task added." << endl;
        sync_call_wait(done, cv, m);
        cout << "OK, sync_call finished." << endl;
    }

    void TestCall() {
        for (int i = 0; i != 10; i++) {
            cout << "timer : " << i << endl;
            boost::this_thread::sleep(boost::posix_time::seconds(1));
        }
    }

    int FunctionB(int a) {
        while (!m_thread_is_running) {
            cout << "thread is not running, wait 1 second" << endl;
            boost::this_thread::sleep(boost::posix_time::seconds(1));
        }

        {
            // add an task.
            sync_call_type f = boost::bind(&TestSyncCallClass::TestCall, this);
            sync_call(f);
        }
    }
    boost::thread* m_thread;
    bool m_thread_quit;
    // Thread is running?
    bool m_thread_is_running;
    std::deque<sync_call_type>m_task_array;
    boost::recursive_mutex m_mutex_task_array;
};

/*
 * 
 */
void TestSyncCall() {
    boost::shared_ptr<TestSyncCallClass> p = boost::make_shared<TestSyncCallClass>();
    p->start();
    p->FunctionB(2);
    cout << "Enter any key to quit." << endl;
    char b;
    cin >> b;
    p->m_thread_quit = true;
    p->m_thread->join();
}

class TestASyncCallClass : public boost::enable_shared_from_this<TestASyncCallClass> {
public:
    typedef boost::function<void() > async_fun;

    TestASyncCallClass() {
        cout << __FUNCTION__ << endl;
        end_thread = false;
        m_thread_started = false;
    }

    ~TestASyncCallClass() {
        cout << __FUNCTION__ << endl;
    }

    async_fun get_task() {
        async_fun c;
        boost::recursive_mutex::scoped_lock a(m_mutex);
        if (!m_tasks.empty()) {
            c = m_tasks.front();
            m_tasks.pop_front();
        }
        return c;
    }

    void thread_function(int nid) {
        cout << "Thread " << nid << " start." << endl;
        m_thread_started = true;
        while (!end_thread) {
            async_fun c = get_task();
            if (!c)
                boost::this_thread::sleep(boost::posix_time::milliseconds(100));
            else {
                c();
            }
        }
    }

    void test_task() {
        for (int i = 0; i != 10; i++) {
            cout << "Test counter:" << i << endl;
        }
    }

    void async_cbk() {
        cout << "Hi, this is async call back." << endl;
    }

    void async_call_proxy(async_fun& f, boost::function<void()>& cbk) {
        f();
        cbk();
    }

    void async_call(async_fun& f, boost::function<void()>& cbk) {
        async_fun c = boost::bind(&TestASyncCallClass::async_call_proxy, this, f, cbk);
        boost::recursive_mutex::scoped_lock a(m_mutex);
        m_tasks.push_back(c);
    }

    void start() {
        if (m_thread)
            return;
        m_thread = boost::make_shared<boost::thread>(&TestASyncCallClass::thread_function, this, 1);
        while(!m_thread_started)
            boost::this_thread::sleep(boost::posix_time::milliseconds(100));
    }

    void AddTestTask() {
        async_fun task = boost::bind(&TestASyncCallClass::test_task, this);
        boost::function<void()> cbk = boost::bind(&TestASyncCallClass::async_cbk, this);
        async_call(task, cbk);
    }

    boost::shared_ptr<boost::thread> m_thread;
    boost::recursive_mutex m_mutex;
    std::deque<async_fun> m_tasks;
    bool end_thread;
    bool m_thread_started;
};

void TestAsyncCall() {
    boost::shared_ptr<TestASyncCallClass> p = boost::make_shared<TestASyncCallClass>();
    p->start();
    p->AddTestTask();
    cout << "Enter any key to quit." << endl;
    char b;
    cin >> b;
    p->end_thread = true;
    p->m_thread->join();
}

int main(int argc, char** argv) {
    //TestSyncCall();
    TestAsyncCall();
    return 0;
}

