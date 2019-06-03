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
#include <exception>
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
        m_thread = boost::make_shared<boost::thread>(&TestSyncCallClass::thread_function, this, 1234);
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
        cout << __FUNCTION__ << " Finish sync call and notify all." << endl;
        cv.notify_all();
    }

    void add_task(sync_call_type& f) {
        boost::recursive_mutex::scoped_lock a(m_mutex_task_array);
        cout << "try add a task." << endl;
        m_task_array.push_back(f);
    }

    void sync_call(sync_call_type& fun) {
        if (boost::this_thread::get_id() == m_thread->get_id()) {
            // `SyncWait` never return if `SyncCall` is called in the work thread.
            // Because the task posted is the next `task_fun_obj_type` object,
            // and you are now try to wait a next executing work complete, but next one will not execute if the current task is not finished.
            throw std::runtime_error("Can not post a sync operation in the thread itself");
            return;
        }
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
    boost::shared_ptr<boost::thread> m_thread;
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

