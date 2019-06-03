/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
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

