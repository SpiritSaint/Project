#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>
#include <boost/program_options.hpp>

struct keys {
    std::string _chained = "chained.pem";
    std::string _public = "public.pem";
    std::string _private = "private.pem";
    std::string _dh_params = "dh-params.pem";
    int _size = 2048;
};

struct http {
    size_t handshake_timeout = 60;
    size_t read_timeout = 60;
    size_t close_timeout = 60;
    std::string server_name = "Project";
};

struct ports {
    uint16_t _serve = 443;
};

class configuration {
public:
    std::string _env_file;
    keys _keys;
    ports _ports;
    http _http;
    void init(boost::program_options::variables_map & options) {
        if (options.count("public_key")) { _keys._public = options.at("public_key").as<std::string>(); }
        if (options.count("private_key")) { _keys._private = options.at("private_key").as<std::string>(); }
        if (options.count("chained_key")) { _keys._chained = options.at("chained_key").as<std::string>(); }
        if (options.count("key_size")) { _keys._size = options.at("key_size").as<int>(); }
        if (options.count("dh_params")) { _keys._dh_params = options.at("dh_params").as<std::string>(); }
        if (options.count("env_file")) { _env_file = options.at("env_file").as<std::string>(); }
        if (options.count("serve")) { _ports._serve = options.at("serve").as<size_t>(); }
    }
};


#endif // CONFIGURATION_H
