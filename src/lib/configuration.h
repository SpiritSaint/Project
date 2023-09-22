#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>
#include <boost/program_options.hpp>

struct rsa {
    std::string _chained_key = "chained.pem";
    std::string _public_key = "public.pem";
    std::string _private_key = "private.pem";
    std::string _dh_params = "dh-params.pem";
    size_t _size = 2048;
};

struct serve {
    uint16_t _port = 8080;
    size_t handshake_timeout = 60;
    size_t read_timeout = 60;
    size_t close_timeout = 60;
    std::string server_name = "Project";
    std::string _directory = "www";
};

class configuration {
public:
    std::string _environment;
    rsa _rsa;
    serve _serve;
    void init(boost::program_options::variables_map & options) {
        if (options.count("encryption_rsa_public_key")) { _rsa._public_key = options.at("encryption_rsa_public_key").as<std::string>(); }
        if (options.count("encryption_rsa_private_key")) { _rsa._private_key = options.at("encryption_rsa_private_key").as<std::string>(); }
        if (options.count("encryption_rsa_chained_key")) { _rsa._chained_key = options.at("encryption_rsa_chained_key").as<std::string>(); }
        if (options.count("encryption_rsa_dh_params")) { _rsa._dh_params = options.at("encryption_rsa_dh_params").as<std::string>(); }
        if (options.count("encryption_rsa_size")) { _rsa._size = options.at("encryption_rsa_size").as<size_t>(); }
        if (options.count("environment")) { _environment = options.at("environment").as<std::string>(); }
        if (options.count("serve_port")) { _serve._port = options.at("serve_port").as<size_t>(); }
        if (options.count("serve_directory")) { _serve._directory = options.at("serve_directory").as<std::string>(); }
    }
};


#endif // CONFIGURATION_H
