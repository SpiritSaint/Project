#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>
#include <boost/program_options.hpp>

struct keys {
    std::string _chained;
    std::string _public;
    std::string _private;
    std::string _dh_params;
    int _size;
};

struct ports {
    uint16_t _serve;
};

class configuration {
public:
    std::string _env_file;
    keys _keys;
    ports _ports;
    void init(boost::program_options::variables_map & options) {
        if (!options.count("public_key")) { _keys._public = "public.pem"; }
        if (!options.count("chained_key")) { _keys._chained = "chained.pem"; }
        if (!options.count("private_key")) { _keys._private = "private.pem"; }
        if (!options.count("key_size")) { _keys._size = 2048; }
        if (!options.count("dh_params")) { _keys._dh_params = "dh-params.pem"; }
        if (!options.count("env_file")) { _env_file = ".env"; }
        if (!options.count("serve")) { _ports._serve = 8080; }
    }
};


#endif // CONFIGURATION_H
