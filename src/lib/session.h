#ifndef SESSION_H
#define SESSION_H

#include "../ext/boost.h"

class session {
public:
    boost::uuids::uuid _uuid;
    std::string _ip;
    boost::asio::ip::port_type _port;
    session() : _uuid(boost::uuids::random_generator()()) {}
};

#endif // SESSION_H
