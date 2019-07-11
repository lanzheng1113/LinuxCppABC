/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "seed_executor.h"
#include "cptype.h"
#include <iostream>
using std::cout;
using std::endl;
#include "torrent_client.h"
#include "util/File.h"

namespace file_seeder {

    seed_executor::seed_executor(const std::string& work_dir, 
            const std::vector<file_seeder::config::seed_tasks_in_conf>& tasks) 
    {
        m_work_dir = work_dir;
        m_tasks_conf = tasks;
    }

    seed_executor::~seed_executor() {

    }

    bool seed_executor::start() {
        m_seed_client = boost::make_shared<file_seeder::torrent_client>();
        m_seed_client->start();
        for (auto i : m_tasks_conf) {
            std::string torrent_file_full_path = m_work_dir + i.torrent_file;
            if (!qcutil::File::exists(torrent_file_full_path)) {
                cout << "Task file " << torrent_file_full_path << " not exist, skip it." << endl;
                continue;
            } else {
                DWORD task_id = m_seed_client->add_task(torrent_file_full_path, m_work_dir);
                if (task_id == 0) {
                    cout << "failed to add task " << torrent_file_full_path << endl;
                    continue;
                }
                cout << "add task " << torrent_file_full_path << " successfully." << endl;
                if (!m_seed_client->start_task(task_id)) {
                    cout << "failed to start task " << torrent_file_full_path << endl;
                    m_seed_client->delete_task(task_id);
                    continue;
                }
                cout << "start task successfully." << endl;
                m_task_id_array.push_back(task_id);
            }
        }
        return true;
    }

    bool seed_executor::stop() {
        // if there are any pending query, wait until it finish.
        m_seed_client->stop();
        return true;
    }
}
