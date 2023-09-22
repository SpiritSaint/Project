#include "websocket_session.h"

websocket_session::websocket_session(boost::beast::ssl_stream<boost::beast::tcp_stream> stream, std::shared_ptr<state> const & state) :
    _stream(std::move(stream)), _state(state) { }

websocket_session::~websocket_session() { }

void websocket_session::fail(boost::system::error_code error, char const * what) {
    if ( error == boost::beast::net::error::operation_aborted || error == boost::beast::websocket::error::closed ) { return; }

    std::cout << "error::websocket_session: " << what << std::endl;
}

void websocket_session::on_accept(boost::system::error_code error) {
    if (error) { return fail(error, "accept"); }

    _stream.async_read(_buffer, boost::beast::bind_front_handler(& websocket_session::on_read, shared_from_this()));
}

void websocket_session::on_read(boost::system::error_code error, std::size_t bytes) {
    std::string content = boost::beast::buffers_to_string(_buffer.data());

    if (error) { return fail(error, "on_read"); }

    _stream.async_read(_buffer, [self = shared_from_this()] (boost::system::error_code error, std::size_t bytes) {
        self->on_read(error, bytes);
    });
}

void websocket_session::send(std::shared_ptr<std::string const> const& stream) {
    _queue.push_back(stream);

    if(_queue.size() > 1)
        return;

    _stream.async_write(boost::asio::buffer(*_queue.front()), [self = shared_from_this()](boost::system::error_code error, std::size_t bytes) {
        self->on_write(error, bytes);
    });
}

void websocket_session::on_write(boost::system::error_code error, std::size_t) {
    if(error)
        return fail(error, "write");

    _queue.erase(_queue.begin());

    if ( !_queue.empty() )
        _stream.async_write(boost::asio::buffer(*_queue.front()), [sp = shared_from_this()] (boost::system::error_code error, std::size_t bytes) {
            sp->on_write(error, bytes);
        });
}

void websocket_session::run(boost::beast::http::request<boost::beast::http::string_body> req) {
    boost::beast::get_lowest_layer(_stream).expires_never();
    _stream.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));
    _stream.set_option(boost::beast::websocket::stream_base::decorator([self = shared_from_this()] (boost::beast::websocket::response_type & res) {
        res.set(boost::beast::http::field::server, self->_state->_config->_http.server_name.c_str());
    }));
    _stream.async_accept(req, std::bind(& websocket_session::on_accept, shared_from_this(), std::placeholders::_1));
}