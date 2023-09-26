#ifndef STATE_H
#define STATE_H

#include "configuration.h"
#include "../ext/bcrypt.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

class websocket_session;

class state {
public:
    std::shared_ptr<configuration> _config;
    std::vector<websocket_session *> _sessions;
    boost::mysql::tcp_ssl_connection _connection;
    state(std::shared_ptr<configuration> config, boost::mysql::tcp_ssl_connection & connection) : _config(std::move(config)), _connection(std::move(connection)) {};
    static std::string get_timestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm * tm = std::gmtime(&time);
        char timestamp[128];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S %Z", tm);
        return std::string{ timestamp };
    }
    bool create(websocket_session * session);
    void mark_as_disconnected(websocket_session * session);
    int email_availability(std::string & email);
    bool do_register(std::string & name, std::string & email, std::string & password);
    std::string do_authentication(std::string & email, std::string & password);
    void send(std::string message, websocket_session * session);
};

#endif // STATE_H
