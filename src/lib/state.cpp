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

void state::send(std::string message, websocket_session * session) {
    boost::json::object event({{"type","broadcast"},{"body",{{"message",message},{"from",boost::lexical_cast<std::string>(session->_uuid)}}}});
    std::string stream { boost::json::serialize(event) };
    auto const ss = std::make_shared<std::string const>(std::move(stream));


    for(auto session : _sessions)
        session->send(ss);
}

int state::email_availability(std::string & email) {
    try {
        boost::mysql::statement stmt = _connection
                .prepare_statement("SELECT email FROM users WHERE email = ?");

        boost::mysql::results result;
        _connection.execute(stmt.bind(email), result);
        return result.rows().size();
    }
    catch (const boost::mysql::error_with_diagnostics & error)
    {
        std::cout << "Operation failed with error code: " << error.code() << '\n'
                  << "Server diagnostics: " << error.get_diagnostics().server_message() << std::endl;
        return -1;
    }
}

bool state::do_register(std::string &name, std::string &email, std::string &password) {
    try {
        auto uuid = boost::uuids::random_generator()();

        boost::mysql::statement stmt = _connection
                .prepare_statement("INSERT INTO users (uuid, name, email, password) VALUES (?, ?, ?, ?)");

        boost::mysql::results result;
        _connection.execute(stmt.bind(boost::lexical_cast<std::string>(uuid), name, email, bcrypt::generateHash(password)), result);
        return true;
    }
    catch (const boost::mysql::error_with_diagnostics & error)
    {
        std::cout << "Operation failed with error code: " << error.code() << '\n'
                  << "Server diagnostics: " << error.get_diagnostics().server_message() << std::endl;
        return false;
    }
}

std::string state::do_authentication(std::string &email, std::string &password) {
    try {
        boost::mysql::statement stmt = _connection
                .prepare_statement("SELECT uuid, email, password FROM users WHERE email = ?");
        boost::mysql::results result;
        _connection.execute(stmt.bind(email), result);
        auto row = result.rows().at(0);
        std::string hash = row.at(2).as_string();
        if (bcrypt::validatePassword(password, hash)) {
            return row.at(0).as_string();
        } else {
            return std::string {"E_PASSWORD"};
        }
    }
    catch (const boost::mysql::error_with_diagnostics & error)
    {
        std::cout << "Operation failed with error code: " << error.code() << '\n'
                  << "Server diagnostics: " << error.get_diagnostics().server_message() << std::endl;
        return std::string {"E_SERVICE"};
    }
}
