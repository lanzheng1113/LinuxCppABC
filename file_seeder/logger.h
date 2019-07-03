/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   logger.h
 * Author: duocore
 *
 * Created on 2019年7月2日, 下午11:19
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <fstream>
#include <string>
#include <sstream>
#include "boost/thread/recursive_mutex.hpp"

namespace file_seeder {

    class logger {
    public:
        ~logger();
        static logger& get_instance();
        void write(const std::string& msg);
    private:
        logger();
        static logger* m_inst;
        std::ofstream m_ofs;
        boost::recursive_mutex m_file_io_mutex;
    };

    class log_helper {
    public:

        log_helper() {
            ;
        }

        ~log_helper() {
            std::string str = m_ss.str();
            if (!str.empty())
                logger::get_instance().write(str);
        };

        std::stringstream& get_log_stream() {
            return m_ss;
        }
    public:
    private:
        std::stringstream m_ss;
    };
}

#ifndef SLOG
#ifdef FILE_SEEDER_LOG_DISABLED
#define SLOG 
#else
#define SLOG file_seeder::log_helper().get_log_stream()
#endif
#endif

#endif /* LOGGER_H */

