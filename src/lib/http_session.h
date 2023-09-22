#ifndef HTTP_SESSION_H
#define HTTP_SESSION_H

#include "../ext/boost.h"
#include "../ext/std.h"

#include "state.h"
#include "websocket_session.h"

class http_session : public std::enable_shared_from_this<http_session> {
    boost::beast::ssl_stream<boost::beast::tcp_stream> _stream;
    boost::asio::ip::tcp::socket _socket;
    boost::beast::flat_buffer _buffer;
    std::shared_ptr<state> _state;
    boost::beast::http::request<boost::beast::http::string_body> _req;
    boost::asio::ssl::context & _context;

    void fail(boost::system::error_code error, char const * what);
    void on_read(boost::system::error_code error, std::size_t length);
    void on_handshake(boost::system::error_code error);
    void on_shutdown(boost::system::error_code error);
    void do_read();
    void do_close();
    void respond(boost::beast::http::message_generator && message);
    void on_write(bool keep_alive, boost::system::error_code error, std::size_t bytes);
public:
    http_session(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context & secure_context, std::shared_ptr<state> state);
    void run();
};


#endif // HTTP_SESSION_H
