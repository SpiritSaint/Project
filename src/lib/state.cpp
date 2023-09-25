#include "state.h"
#include "websocket_session.h"

bool state::create(websocket_session * session)  {
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

void state::mark_as_disconnected(websocket_session *session) {
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

void state::send(std::string message) {
    auto const ss = std::make_shared<std::string const>(std::move(message));
    for(auto session : _sessions)
        session->send(ss);
}
