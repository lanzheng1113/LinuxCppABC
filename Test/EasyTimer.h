#pragma once
#include "boost/thread.hpp"
#include "boost/function.hpp"
#include "boost/thread/recursive_mutex.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"

typedef boost::function<void() > timer_cbk;
typedef boost::function<void() > task_fun_obj_type;

class EasyTimer : public boost::noncopyable {
public:

    EasyTimer() {
        m_bStarted = true;
        m_thd = boost::make_shared<boost::thread>(&EasyTimer::thread_function, this);
    }

    ~EasyTimer() {
        m_bStarted = false;
        if (m_thd->joinable()) {
            m_thd->join();
        }
    }

    struct TimerIMP {
        int id;
        int inter_val;
        timer_cbk cbk;
        boost::posix_time::ptime next_triggle_time;
    };

public:

    void SetTimer(int id, int milli_seconds, const timer_cbk& cbk) {
        TimerIMP p;
        p.id = id;
        p.inter_val = milli_seconds;
        p.cbk = cbk;
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        p.next_triggle_time = now + boost::posix_time::milliseconds(milli_seconds);
        boost::recursive_mutex::scoped_lock l(m_mu);
        m_Timers.push_back(p);
    }

    void KillTimer(int id) {
        boost::recursive_mutex::scoped_lock l(m_mu);
        for (std::vector<TimerIMP>::iterator it = m_Timers.begin(); it != m_Timers.end(); ++it) {
            if (it->id == id) {
                m_Timers.erase(it);
                break;
            }
        }
    }
private:

    void ResetTimer(int id) {
        boost::recursive_mutex::scoped_lock l(m_mu);
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        for (TimerIMP& i : m_Timers) {
            if (i.id == id) {
                i.next_triggle_time = now + boost::posix_time::milliseconds(i.inter_val);
                break;
            }
        }
    }

    task_fun_obj_type PopTimerTask(int& id) {
        boost::recursive_mutex::scoped_lock l(m_mu);
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        task_fun_obj_type ret;
        for (auto i : m_Timers) {
            if (now > i.next_triggle_time) {
                id = i.id;
                ret = i.cbk;
                break;
            }
        }
        return ret;
    }
protected:

    void thread_function() {
        while (m_bStarted) {
            int id = 0;
            task_fun_obj_type t = PopTimerTask(id);
            if (!t) {
                boost::this_thread::sleep(boost::posix_time::milliseconds(100));
                continue;
            } else {
                //
                // Note we must reset the timer next triggle time ONLY AFTER the callback function has called.
                // Because if a task use many many time, the other task may be have no chance to execute.
                //
                t(); // <== break here.
                ResetTimer(id);
            }

        }
    }

private:
    boost::recursive_mutex m_mu;
    std::vector<TimerIMP> m_Timers;
    boost::shared_ptr<boost::thread> m_thd;
    bool m_bStarted;
};