#ifndef HTTP_LISTENER_H
#define HTTP_LISTENER_H

#include <iostream>
#include <memory>
#include <utility>

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ssl/stream.hpp>

#include "http_session.h"

class state;

class http_listener : public std::enable_shared_from_this<http_listener> {
    boost::asio::io_context & _io_context;
    boost::asio::ip::tcp::acceptor _acceptor;
    boost::asio::ip::tcp::socket _socket;
    boost::asio::ssl::context & _secure_context;
    static void fail(boost::system::error_code error, char const * what);
    void on_accept(boost::system::error_code error);
public:
    std::shared_ptr<state> _state;
    http_listener(boost::asio::io_context & io_context, boost::asio::ssl::context & secure_context, boost::asio::ip::tcp::endpoint & endpoint, std::shared_ptr<state> const & state);
    void run();
};


#endif // HTTP_LISTENER_H
