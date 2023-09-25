#include "websocket_session.h"

websocket_session::websocket_session(boost::beast::ssl_stream<boost::beast::tcp_stream> stream, std::shared_ptr<state> const & state) : _stream(std::move(stream)), _state(state) { }

websocket_session::~websocket_session() {
}

void websocket_session::fail(boost::system::error_code error, char const * what) {
    _state->mark_as_disconnected(_session);
    if ( error == boost::beast::net::error::operation_aborted || error == boost::beast::websocket::error::closed ) { return; }
}

void websocket_session::on_accept(boost::system::error_code error) {
    if ( error ) { return fail(error, "accept"); }

    auto remote_endpoint = boost::beast::get_lowest_layer(_stream).socket().remote_endpoint();
    _session = new session;
    _session->_ip = remote_endpoint.address().to_string();
    _session->_port = remote_endpoint.port();

    if (_state->create(_session)) {
        std::string separator = " ";
        std::cout << GREEN << state::get_timestamp() << RESET << separator << BOLD_GREEN << "EVT" << RESET << separator << BOLD_BLUE << _session->_ip << ":" << _session->_port <<  RESET << separator << BOLD_CYAN << "Connected" << RESET << std::endl;
        _stream.async_read(_buffer, boost::beast::bind_front_handler(& websocket_session::on_read, shared_from_this()));
    } else {
        return fail(error, "accept_database");
    }
}

void websocket_session::on_read(boost::system::error_code error, std::size_t bytes) {
    std::string content = boost::beast::buffers_to_string(_buffer.data());
    _buffer.consume(_buffer.size());

    if (error) { return fail(error, "on_read"); }

    if (json_validator::check(content)) {
        std::shared_ptr<const std::string> response = std::make_shared<const std::string>("200");
        std::string separator = " ";
        this->send(response);
        std::cout << GREEN << state::get_timestamp() << RESET << separator << BOLD_GREEN << "MSG" << RESET << separator << BOLD_BLUE << content << RESET << separator << BOLD_CYAN << *response << RESET << std::endl;
    } else {
        std::shared_ptr<const std::string> response = std::make_shared<const std::string>("500");
        std::string separator = " ";
        this->send(response);
        std::cout << GREEN << state::get_timestamp() << RESET << separator << BOLD_GREEN << "MSG" << RESET << separator << BOLD_BLUE << content << RESET << separator << BOLD_CYAN << *response << RESET << std::endl;
    }


    _stream.async_read(_buffer, boost::beast::bind_front_handler(& websocket_session::on_read, shared_from_this()));
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
    if (error) return fail(error, "write");

    _queue.erase(_queue.begin());

    if ( !_queue.empty() )
        _stream.async_write(boost::asio::buffer(*_queue.front()), [sp = shared_from_this()] (boost::system::error_code error, std::size_t bytes) {
            sp->on_write(error, bytes);
        });
}

void websocket_session::run(boost::beast::http::request<boost::beast::http::string_body> request) {
    boost::beast::get_lowest_layer(_stream).expires_never();
    _stream.set_option(boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));
    _stream.set_option(boost::beast::websocket::stream_base::decorator([self = shared_from_this()] (boost::beast::websocket::response_type & response) {
        response.set(boost::beast::http::field::server, self->_state->_config->_serve._server_name.c_str());
    }));
    _stream.async_accept(request, std::bind(& websocket_session::on_accept, shared_from_this(), std::placeholders::_1));
}