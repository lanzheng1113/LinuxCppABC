/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "sync_call.h"
#include "torrent_client.h"
#include "libtorrent/session.hpp"
#include "libtorrent/alert.hpp"
#include "libtorrent/alert_types.hpp"
#include "libtorrent/torrent_info.hpp"
#include "easy_timer.h"
#include "logger.h"

namespace lt = libtorrent;

namespace file_seeder {

    // the setting of libtorrent session.

    static lt::settings_pack default_settings() {
        using lt::alert;
        using lt::settings_pack;

        const int mask = 0
                //| libtorrent::alert::stats_notification		// `stats` for statistics
                | lt::alert::status_notification
                | lt::alert::port_mapping_notification
#ifdef MORE_DETAIL_LOG
                | lt::alert::torrent_log_notification
                | lt::alert::session_log_notification
                | lt::alert::dht_log_notification
                | lt::alert::peer_log_notification
                | lt::alert::port_mapping_log_notification //open this option to show the log generated in mapping progress.
#endif // DEBUG
                ;
        lt::settings_pack pack;
        // the app will run on server as a seeder, so we choose the `high performance seed` setting.
        lt::high_performance_seed(pack);
        pack.set_int(settings_pack::alert_mask, mask);
        pack.set_bool(settings_pack::enable_lsd, true);
        pack.set_bool(settings_pack::enable_natpmp, true);
        pack.set_bool(settings_pack::enable_upnp, true);
        pack.set_bool(settings_pack::enable_dht, true);
        pack.set_str(settings_pack::user_agent, "txk/1.1.8");
        pack.set_str(settings_pack::listen_interfaces, "0.0.0.0:3799"); //use a fixed port 3799.
        pack.set_str(settings_pack::dht_bootstrap_nodes, "dht.libtorrent.org:25401,router.utorrent.com:6881,router.bittorrent.com:6881,dht.transmissionbt.com:6881,router.bitcomet.com:6881,dht.aelitis.com:6881");
        pack.set_bool(settings_pack::allow_multiple_connections_per_ip, true);
        pack.set_int(settings_pack::network_threads, 8);
        pack.set_int(settings_pack::aio_threads, 4);
        return pack;
    }

    void torrent_client::create_session() {
        lt::settings_pack sp = default_settings();
        m_torrent_session = boost::make_shared<lt::session>(sp);
    };

    static const int timer_id_process_alert = 101;

    bool torrent_client::start() {
        if (m_torrent_session)
            return false;
        m_last_post_status_update = lt::clock_type::now();
        m_torrent_worker = boost::make_shared<sync_caller>();
        sync_caller::sync_call_type t = boost::bind(&torrent_client::create_session, this);
        m_torrent_worker->do_call(t);
        //start a timer thread to retrive the alert.
        m_alert_process_timer = boost::make_shared<easy_timer>();
        m_alert_process_timer->set_timer(timer_id_process_alert, 250, [ = ](){on_alert();});
        //
        // OK now the session is start, and we can receive the alert in `on_alert` function.
        //
        return true;
    }

    bool torrent_client::stop() {
        if (!m_torrent_session)
            return false;
        m_alert_process_timer->kill_timer(timer_id_process_alert);
        m_alert_process_timer.reset(); // will wait untill the current task is finished, and discard the other tasks remain in the task queue.
        sync_caller::sync_call_type t = [ = ](){m_torrent_session.reset();};
        m_torrent_worker->do_call(t);
        m_torrent_worker->stop();
        m_torrent_worker.reset();
        return true;
    }

    void torrent_client::on_alert() {
        if (!m_torrent_session)
            return;
        lt::time_point current_time_point = lt::clock_type::now();
        if (current_time_point - m_last_post_status_update >= lt::milliseconds(1000)) {
            m_last_post_status_update = current_time_point;
            // Call post_torrent_updates so that we can receive a `state_update_alert`.
            m_torrent_session->post_torrent_updates();
            // Call post_session_stats to retrieve a `session_stats_alert`.
            //m_torrent_session->post_session_stats();
        }

        std::vector<lt::alert*> alerts;
        m_torrent_session->pop_alerts(&alerts);
        if (alerts.size() > 0) {
            std::vector<std::auto_ptr < lt::alert>> alerts_clone;
            for (int i = 0; i != alerts.size(); i++) {
                alerts_clone.push_back(alerts[i]->clone());
            }
            
            for (int i = 0; i != alerts_clone.size(); i++) {
                std::auto_ptr<lt::alert> alert = alerts_clone[i];
                if (alert->type() == lt::state_changed_alert::alert_type) {
                    lt::state_changed_alert* o = static_cast<lt::state_changed_alert*> (alert.get());
                    // record the task status to our task list
                    task_sptr task = get_task_from_sha1_hash(o->handle.info_hash());
                    if (task)
                    {
                        task->status.state = o->state;
                        if (o->state == lt::torrent_status::downloading_metadata) {
                            ;
                        } else if (o->state == lt::torrent_status::downloading) {
                            ;
                        } else if (o->state == lt::torrent_status::finished || o->state == lt::torrent_status::seeding) {
                            //finished;
                        }
                    }
                    //switch (o->state) {
                    //case torrent_status::queued_for_checking:
                    //	break;
                    //case torrent_status::checking_files:
                    //	break;
                    //case torrent_status::downloading_metadata:
                    //	break;
                    //case torrent_status::downloading:
                    //	break;
                    //case torrent_status::finished:
                    //	break;
                    //case torrent_status::seeding:
                    //	break;
                    //case torrent_status::allocating:
                    //	break;
                    //case torrent_status::checking_resume_data:
                    //	break;
                    //}
                } else if (alert->type() == lt::add_torrent_alert::alert_type) {
                    lt::add_torrent_alert* o = static_cast<lt::add_torrent_alert*> (alert.get());
                    task_sptr task = get_task_from_sha1_hash(o->handle.info_hash());
                    if (task) {
                        lt::add_torrent_params params = o->params;
                        char info_hash[41];
                        char const* torrent_name = info_hash;
                        if (params.ti)
                            torrent_name = params.ti->name().c_str();
                        else if (!params.name.empty())
                            torrent_name = params.name.c_str();
                        else if (!params.url.empty())
                            torrent_name = params.url.c_str();
                        else
                            lt::to_hex(params.info_hash.data(), 20, info_hash);
                        task->torrent_name = torrent_name;
                    }
                } else if (alert->type() == lt::state_update_alert::alert_type) {
                    lt::state_update_alert* o = static_cast<lt::state_update_alert*> (alert.get());
                    for (auto it : o->status) {
                        task_sptr task = get_task_from_sha1_hash(it.info_hash);
                        if (task) {
                            task->status = it;  //update status.
                        }
                    }
                } else if (alert->type() == lt::torrent_deleted_alert::alert_type) {
                    lt::torrent_deleted_alert* o = static_cast<lt::torrent_deleted_alert*> (alert.get());
                    remove_task_from_tasklist(o->info_hash);
                } else if (alert->type() == lt::torrent_paused_alert::alert_type) {
                    //
                    // We will receive a ``torrent_paused_alert`` when ``task_stop()`` is called.
                    // Note: 
                    // 1) lt::state_update_alert and lt::state_changed_alert never come when `task_stop()` was called.
                    // 2) The operation of pausing a session never post a `torrent_paused_alert` alert.
                    //
                    lt::torrent_paused_alert* o = static_cast<lt::torrent_paused_alert*> (alert.get());
                    task_sptr task = get_task_from_sha1_hash(o->handle.info_hash());
                    if (task) {
                        task->status = o->handle.status();
                    }
                } else if (alert->type() == lt::portmap_alert::alert_type) {
                    lt::portmap_alert* o = static_cast<lt::portmap_alert*> (alert.get());
                    // check mapped protocol
                    if ((o->external_port <= 0)
                            || (o->protocol != lt::portmap_alert::tcp && o->protocol != lt::portmap_alert::udp)
                            || (o->map_type != MAP_TYPE_NAT_PMP && o->mapping != MAP_TYPE_UPNP)) {
                        //
                        // We do not need this alert except the type is udp or tcp.
                        // 
                    } else {
                        // notify mapped
                        if (o->map_type == MAP_TYPE_NAT_PMP) {
                            if (o->protocol == lt::portmap_alert::tcp) {
                                m_local_service_info.portmap_extern_tcp_port = o->external_port;
                            } else if (o->protocol == lt::portmap_alert::udp) {
                                m_local_service_info.portmap_extern_udp_port = o->external_port;
                            } else {
                                ;
                            }
                        } else if (o->map_type == MAP_TYPE_UPNP) {
                            if (o->protocol == lt::portmap_alert::tcp) {
                                m_local_service_info.portmap_extern_tcp_port = o->external_port;
                            } else if (o->protocol == lt::portmap_alert::udp) {
                                m_local_service_info.portmap_extern_udp_port = o->external_port;
                            } else {
                                ;
                            }
                        }
                    }
                } else if (alert->type() == lt::listen_succeeded_alert::alert_type) {
                    lt::listen_succeeded_alert* o = static_cast<lt::listen_succeeded_alert*> (alert.get());
                    //tcp, udp
                    if (o->sock_type == lt::listen_succeeded_alert::tcp) {
                        m_local_service_info.listen_succeeded_tcp_port = o->endpoint.port();
                    } else if (o->sock_type == lt::listen_succeeded_alert::udp) {
                        m_local_service_info.listen_succeeded_udp_port = o->endpoint.port();
                    } else {
                        SLOG(info) << "[unsupported socket type]" << alert->what() << ";" << alert->message() << std::endl;
                    }
                } else if (alert->type() == lt::listen_failed_alert::alert_type) {
                    //just print it.
                    lt::listen_failed_alert* o = static_cast<lt::listen_failed_alert*> (alert.get());
                    std::string t = o->message();
                    //t = lt::convert_to_native(t);
                    SLOG(info) << alert->what() << ";" << t << std::endl;
                } else if (alert->type() == lt::portmap_error_alert::alert_type) {
                    //just print it.
                    lt::portmap_error_alert* o = static_cast<lt::portmap_error_alert*> (alert.get());
                    std::string t = o->message();
                    SLOG(info) << alert->what() << ";" << t << std::endl;
                } else if (alert->type() == lt::external_ip_alert::alert_type) {
                    lt::external_ip_alert* o = static_cast<lt::external_ip_alert*> (alert.get());
                    //
                    // Recored the extern ip to log.
                    // Note: the external ip may be changed during downloading, even more than once.
                    //
                    m_local_service_info.external_ip = o->external_address.to_string();
                }

                if (alert->type() != lt::state_update_alert::alert_type) {
                    //Log alert information.
                    SLOG(info) << alert->what() << ";" << alert->message() << std::endl;
                }

                if (m_alert_callback) {
                    std::string str = alert->what();
                    str += ";";
                    str += alert->message();
                    //str += lt::convert_to_native(alert->message());
                    m_alert_callback( str, alert->type());
                }
            }
        }
    }
}