#ifndef STATE_H
#define STATE_H

#include "configuration.h"

class state {
public:
    std::shared_ptr<configuration> _config;
    state(std::shared_ptr<configuration> config) : _config(std::move(config)) {};
    static std::string get_timestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::tm * tm = std::gmtime(&time);
        char timestamp[128];
        std::strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S %Z", tm);
        return std::string{ timestamp };
    }
};

#endif // STATE_H
