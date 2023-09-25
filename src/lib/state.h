#ifndef STATE_H
#define STATE_H

#include "configuration.h"
#include "session.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>

class state {
public:
    std::shared_ptr<configuration> _config;
    std::vector<session *> _sessions;
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
    bool create(session * session) {
        _sessions.push_back(session);
        try {
            boost::mysql::statement stmt = _connection
                    .prepare_statement("INSERT INTO sessions (uuid, ip, port) VALUES (?, ?, ?)");

            boost::mysql::results result;
            _connection.execute(stmt.bind(boost::lexical_cast<std::string>(session->_uuid), session->_ip, session->_port), result);
            return true;
        }
        catch (const boost::mysql::error_with_diagnostics & error)
        {
            std::cout << "Operation failed with error code: " << error.code() << '\n'
                      << "Server diagnostics: " << error.get_diagnostics().server_message() << std::endl;
            return false;
        }
    }
    void mark_as_disconnected(session * session) {
        std::string separator = " ";
        for (size_t i = 0; i < _sessions.size(); ++i) {
            if (_sessions[i]->_uuid == session->_uuid) {
                _sessions.erase(_sessions.begin() + i);

                try {
                    boost::mysql::statement stmt = _connection
                            .prepare_statement("UPDATE sessions SET disconnected_at = now() WHERE uuid = ?");

                    boost::mysql::results result;
                    _connection.execute(stmt.bind(boost::lexical_cast<std::string>(session->_uuid)), result);

                    std::cout << GREEN << state::get_timestamp() << RESET << separator << BOLD_GREEN << "EVT" << RESET << separator << BOLD_BLUE << session->_ip << ":" << session->_port <<  RESET << separator << BOLD_CYAN << "Disconnected" << RESET << std::endl;
                }
                catch (const boost::mysql::error_with_diagnostics & error)
                {
                    std::cout << "Operation failed with error code: " << error.code() << '\n'
                              << "Server diagnostics: " << error.get_diagnostics().server_message() << std::endl;
                }

                break;
            }
        }
    }
};

#endif // STATE_H
