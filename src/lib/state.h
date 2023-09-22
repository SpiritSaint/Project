#ifndef STATE_H
#define STATE_H

#include "configuration.h"

class state {
public:
    std::shared_ptr<configuration> _config;
    state(std::shared_ptr<configuration> config) : _config(std::move(config)) {};
};

#endif // STATE_H
