#include "http_listener.h"

http_listener::http_listener(boost::asio::io_context &io_context,
                             boost::asio::ssl::context & secure_context,
                             boost::asio::ip::tcp::endpoint &endpoint,
                             std::shared_ptr<state> const & state)
        : _io_context(io_context), _acceptor(io_context), _socket(io_context), _secure_context(secure_context), _state(state) {
    boost::system::error_code error;

    _acceptor.open(endpoint.protocol(), error);
    if (error) {
        fail(error, "open");
        return;
    }

    _acceptor.set_option(boost::asio::socket_base::reuse_address(true), error);
    if (error) {
        fail(error, "set_option");
        return;
    }

    _acceptor.bind(endpoint, error);
    if (error) {
        fail(error, "bind");
        return;
    }

    _acceptor.listen(boost::asio::socket_base::max_listen_connections, error);
    if(error) {
        fail(error, "listen");
        return;
    }
}

void http_listener::run() {
    _acceptor.async_accept(_socket, [self = shared_from_this()](boost::system::error_code error) {
        self->on_accept(error);
    });
}

void http_listener::on_accept(boost::system::error_code error) {
    if (error) {
        return fail(error, "accept");
    } else {
        std::make_shared<http_session>(std::move(_socket),_secure_context, _state)->run();
    }

    _acceptor.async_accept(_socket, [self = shared_from_this()](boost::system::error_code error) {
        self->on_accept(error);
    });
}

void http_listener::fail(boost::system::error_code error, char const * what) {
    if(error == boost::asio::error::operation_aborted)
        return;
}
