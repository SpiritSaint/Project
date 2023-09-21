#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>
#include <boost/program_options.hpp>

struct keys {
    std::string _public;
    std::string _private;
};

class configuration {
public:
    keys _keys;
    void init(boost::program_options::variables_map & options) {
        if (!options.count("public_key")) { _keys._public = "public.pem"; }
        if (!options.count("private_key")) { _keys._private = "private.pem"; }
    }
};


#endif // CONFIGURATION_H
