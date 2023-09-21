#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <iostream>

struct keys {
    std::string _public;
    std::string _private;
};

class configuration {
public:
    keys _keys;
};


#endif // CONFIGURATION_H
