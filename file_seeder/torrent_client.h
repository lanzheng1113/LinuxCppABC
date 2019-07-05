/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   torrent_client.h
 * Author: duocore
 *
 * Created on 2019年7月2日, 下午11:17
 */

#ifndef TORRENT_CLIENT_H
#define TORRENT_CLIENT_H

#include <boost/smart_ptr/shared_ptr.hpp>
#include <libtorrent/torrent_status.hpp>
#include <libtorrent/sha1_hash.hpp>
#include "libtorrent/time.hpp"
#include <map>
#include "cptype.h"

namespace libtorrent {
    class session;
}

namespace lt = libtorrent;

namespace file_seeder {

    class client_task {
    public:
        client_task()
        {
            id = 0;
        }
    public:
        // The task id to instead of `torrent_hash` member, because `torrent_hash` is a big data. it start from 1 and 0 is reserve to represent a `bad id`.
        int id;
        // The current status of the task.
        lt::torrent_status status;
        // The hash-info of the torrent in current session.
        lt::sha1_hash torrent_hash;
        // search `lt::add_torrent_alert::alert_type` in cpp to get more information. 
        //It is one of lt::add_torrent_params::ti::name / lt::add_torrent_params::name / lt::add_torrent_params::url / lt::add_torrent_params::hash_info
        std::string torrent_name;
    };
    
    struct P2P_LOCAL_SERVICE_INFO {
        int portmap_extern_tcp_port;
        int portmap_extern_udp_port;
        int listen_succeeded_tcp_port;
        int listen_succeeded_udp_port;
        std::string external_ip;
    };
    
    class sync_caller;
    class easy_timer;
    typedef boost::shared_ptr<client_task> task_sptr;
    typedef std::map<lt::sha1_hash, task_sptr> task_map;

    class torrent_client {

        enum {
            MAP_TYPE_NAT_PMP = 0, // 0 for NAT-PMP and 1 for UPnP.
            MAP_TYPE_UPNP = 1, // 0 for NAT-PMP and 1 for UPnP.
        };
    public:
        bool start();
        bool stop();
        DWORD add_torrent(const std::string& url, const std::string& save_path);
    private:
        // implement `add_torrent` this function will be called in sync-caller.
        DWORD add_torrent_imp(const std::string& url, const std::string& save_path);

        void create_session();
        void on_alert();

        task_sptr get_task_from_sha1_hash(const lt::sha1_hash& hash) {
            task_map::iterator it = m_tasks.find(hash);
            if (it != m_tasks.end()) {
                return it->second;
            } else {
                return task_sptr();
            }
        }

        void remove_task_from_tasklist(const lt::sha1_hash& hash) {
            task_map::iterator it = m_tasks.find(hash);
            if (it != m_tasks.end())
                m_tasks.erase(it);
        }

    private:
        // most of the libtorrent function will be called in this caller(thread).
        boost::shared_ptr<sync_caller> m_torrent_worker;
        // the libtorrent session.
        boost::shared_ptr<lt::session> m_torrent_session;
        // the timer (thread) to retrive and process alert.
        boost::shared_ptr<easy_timer> m_alert_process_timer;
        // last time we post a status update request.
        lt::time_point m_last_post_status_update;
        // Save a task list add to libtorrent session, and recored its status in it.
        task_map m_tasks;
        // Save the port mapping, the listen port, the extern ip information.
        P2P_LOCAL_SERVICE_INFO m_local_service_info;
        // provide a callback function, it will be called when a new alert comes.
        // The first parameter is the alert message (utf-8 encode).and the second one is alert type.
        boost::function<void (const std::string&, int)> m_alert_callback;
    };
}

#endif /* TORRENT_CLIENT_H */

