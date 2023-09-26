#ifndef WEBSOCKET_SESSION_H
#define WEBSOCKET_SESSION_H

#include "../ext/boost.h"
#include "../ext/std.h"
#include "json.h"
#include "state.h"

class state;

class websocket_session : public std::enable_shared_from_this<websocket_session> {
    boost::beast::flat_buffer _buffer;
    boost::beast::websocket::stream<boost::beast::ssl_stream<boost::beast::tcp_stream>> _stream;
    std::shared_ptr<state> _state;
    std::vector<std::shared_ptr<std::string const>> _queue;

    void fail(boost::system::error_code error, char const * what);
    void on_accept(boost::system::error_code error);
    void on_read(boost::system::error_code error, std::size_t bytes);
    void on_write(boost::system::error_code error, std::size_t bytes);
public:
    boost::uuids::uuid _uuid;
    std::string _ip;
    boost::asio::ip::port_type _port;
    websocket_session(boost::beast::ssl_stream<boost::beast::tcp_stream> stream, std::shared_ptr<state> const & state);
    ~websocket_session();
    void run(boost::beast::http::request<boost::beast::http::string_body> request);
    void send(std::shared_ptr<std::string const> const & stream);
    void print(std::string & message, std::string response);
};


#endif // WEBSOCKET_SESSION_H
