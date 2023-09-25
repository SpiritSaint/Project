#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>
#include "../ext/boost.h"

struct rsa {
    std::string _chained_key = "chained.pem";
    std::string _public_key = "public.pem";
    std::string _private_key = "private.pem";
    std::string _dh_params = "dh-params.pem";
    size_t _size = 2048;
};

struct database {
    std::string _hostname = "localhost";
    std::string _name = "project";
    std::string _username = "user";
    std::string _password = "password";
};

struct serve {
    uint16_t _port = 8080;
    size_t _handshake_timeout = 60;
    size_t _read_timeout = 60;
    size_t _close_timeout = 60;
    std::string _server_name = "Project";
    std::string _directory = "www";
};

class configuration {
public:
    std::string _environment;
    rsa _rsa;
    serve _serve;
    database _database;
    void init(boost::program_options::variables_map & options) {
        if (options.count("encryption_rsa_public_key")) { _rsa._public_key = options.at("encryption_rsa_public_key").as<std::string>(); }
        if (options.count("encryption_rsa_private_key")) { _rsa._private_key = options.at("encryption_rsa_private_key").as<std::string>(); }
        if (options.count("encryption_rsa_chained_key")) { _rsa._chained_key = options.at("encryption_rsa_chained_key").as<std::string>(); }
        if (options.count("encryption_rsa_dh_params")) { _rsa._dh_params = options.at("encryption_rsa_dh_params").as<std::string>(); }
        if (options.count("encryption_rsa_size")) { _rsa._size = options.at("encryption_rsa_size").as<size_t>(); }
        if (options.count("environment")) { _environment = options.at("environment").as<std::string>(); }
        if (options.count("serve_port")) { _serve._port = options.at("serve_port").as<size_t>(); }
        if (options.count("serve_directory")) { _serve._directory = options.at("serve_directory").as<std::string>(); }
        if (options.count("database_hostname")) { _database._hostname = options.at("database_hostname").as<std::string>(); }
        if (options.count("database_name")) { _database._name = options.at("database_name").as<std::string>(); }
        if (options.count("database_username")) { _database._username = options.at("database_username").as<std::string>(); }
        if (options.count("database_password")) { _database._password = options.at("database_password").as<std::string>(); }
    }
    boost::mysql::tcp_ssl_connection get_connection() {
        boost::asio::io_context mysql_context;
        boost::asio::ssl::context mysql_secure_context(boost::asio::ssl::context::tls_client);
        boost::mysql::tcp_ssl_connection connection(mysql_context.get_executor(), mysql_secure_context);
        boost::asio::ip::tcp::resolver resolver(mysql_context.get_executor());
        auto endpoints = resolver.resolve(_database._hostname.c_str(), boost::mysql::default_port_string);
        boost::mysql::handshake_params params(
                _database._username.c_str(),
                _database._password.c_str(),
                _database._name.c_str()
        );
        connection.connect(*endpoints.begin(), params);
        return connection;
    }
};


#endif // CONFIGURATION_H
