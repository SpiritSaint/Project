#include "websocket_session.h"

websocket_session::websocket_session(boost::beast::ssl_stream<boost::beast::tcp_stream> stream, std::shared_ptr<state> const & state) : _stream(std::move(stream)), _state(state), _uuid(boost::uuids::random_generator()()) { }

websocket_session::~websocket_session() {
}

void websocket_session::fail(boost::system::error_code error, char const * what) {
    _state->mark_as_disconnected(this);
    if ( error == boost::beast::net::error::operation_aborted || error == boost::beast::websocket::error::closed ) { return; }
}

void websocket_session::on_accept(boost::system::error_code error) {
    if ( error ) { return fail(error, "accept"); }

    auto remote_endpoint = boost::beast::get_lowest_layer(_stream).socket().remote_endpoint();
    _ip = remote_endpoint.address().to_string();
    _port = remote_endpoint.port();

    if (_state->create(this)) {
        std::string separator = " ";
        std::cout << GREEN << state::get_timestamp() << RESET << separator << BOLD_GREEN << "EVT" << RESET << separator << BOLD_BLUE << _ip << ":" << _port <<  RESET << separator << BOLD_CYAN << "Connected" << RESET << std::endl;
        _stream.async_read(_buffer, boost::beast::bind_front_handler(& websocket_session::on_read, shared_from_this()));
    } else {
        return fail(error, "accept_database");
    }
}

void websocket_session::print(std::string & message, std::string response) {
    std::string separator = " ";
    std::cout << GREEN << state::get_timestamp() << RESET << separator << BOLD_GREEN << "MSG" << RESET << separator << BOLD_BLUE << message << RESET << separator << BOLD_CYAN << response << RESET << std::endl;
}

void websocket_session::on_read(boost::system::error_code error, std::size_t bytes) {
    std::string content = boost::beast::buffers_to_string(_buffer.data());
    _buffer.consume(_buffer.size());

    if (error) { return fail(error, "on_read"); }

    boost::json::value value = boost::json::parse(content, error);

    std::string separator = " ";
    if (error) {
        std::shared_ptr<const std::string> response = std::make_shared<const std::string>("500");
        this->send(response);
        this->print(content, response->c_str());
    } else {
        boost::json::object object = value.as_object();

        if (object.contains("type") && object.contains("body") && object.at("body").is_object()) {
            std::string type { object.at("type").as_string() };
            boost::json::object body { object.at("body").as_object() };

            if (type == "broadcast") {
                if (body.contains("message") && body.at("message").is_string()) {
                    std::string message { body.at("message").as_string() };
                    _state->send(message, this);
                    std::shared_ptr<const std::string> response = std::make_shared<const std::string>("200");
                    this->send(response);
                    this->print(content, response->c_str());
                } else {
                    std::shared_ptr<const std::string> response = std::make_shared<const std::string>("422:{\"rules\":[\"body.message is required\",\"body.message must be an string\"]}");
                    this->send(response);
                    this->print(content, response->c_str());
                }
            } else if (type == "register") {
                if (body.contains("name") && body.at("name").is_string() && body.contains("email") && body.at("email").is_string() && body.contains("password") && body.at("password").is_string()) {
                    std::string name { body.at("name").as_string() };
                    std::string email { body.at("email").as_string() };
                    std::string password { body.at("password").as_string() };
                    auto availability = _state->email_availability(email);
                    if (availability == 0) {
                        _state->do_register(name, email, password);
                        std::shared_ptr<const std::string> response = std::make_shared<const std::string>("200");
                        this->send(response);
                        this->print(content, response->c_str());
                    } else if (availability == -1) {
                        std::shared_ptr<const std::string> response = std::make_shared<const std::string>("501:{\"details\":\"service unavailable\"}");
                        this->send(response);
                        this->print(content, response->c_str());
                    } else {
                        std::shared_ptr<const std::string> response = std::make_shared<const std::string>("501:{\"details\":\"email registered\"}");
                        this->send(response);
                        this->print(content, response->c_str());
                    }
                } else {
                    std::shared_ptr<const std::string> response = std::make_shared<const std::string>("422:{\"rules\":[\"body.name is required\",\"body.name must be an string\",\"body.email is required\",\"body.email must be an string\",\"body.password is required\",\"body.password must be an string\"]}");
                    this->send(response);
                    this->print(content, response->c_str());
                }
            } else if (type == "auth") {
                if (body.contains("email") && body.at("email").is_string() && body.contains("password") && body.at("password").is_string()) {
                    std::string email { body.at("email").as_string() };
                    std::string password { body.at("password").as_string() };
                    auto availability = _state->email_availability(email);
                    if (availability == 1) {
                        std::string uuid = _state->do_authentication(email, password);
                        if (uuid == "E_PASSWORD") {
                            std::shared_ptr<const std::string> response = std::make_shared<const std::string>("500:{\"details\":\"password incorrect\"}");
                            this->send(response);
                            this->print(content, response->c_str());
                        } else if (uuid == "E_SERVICE") {
                            std::shared_ptr<const std::string> response = std::make_shared<const std::string>("500:{\"details\":\"service unavailable\"}");
                            this->send(response);
                            this->print(content, response->c_str());
                        } else {
                            std::shared_ptr<const std::string> response = std::make_shared<const std::string>("200");
                            this->send(response);
                            this->print(content, response->c_str());
                        }
                    } else if (availability == -1) {
                        std::shared_ptr<const std::string> response = std::make_shared<const std::string>("500:{\"details\":\"service unavailable\"}");
                        this->send(response);
                        this->print(content, response->c_str());
                    } else {
                        std::shared_ptr<const std::string> response = std::make_shared<const std::string>("500:{\"details\":\"email not found\"}");
                        this->send(response);
                        this->print(content, response->c_str());
                    }
                } else {
                    std::shared_ptr<const std::string> response = std::make_shared<const std::string>("422:{\"rules\":[\"body.email is required\",\"body.email must be an string\",\"body.password is required\",\"body.password must be an string\"]}");
                    this->send(response);
                    this->print(content, response->c_str());
                }
            } else {
                std::shared_ptr<const std::string> response = std::make_shared<const std::string>("501:{\"details\":\"not implemented\"}");
                this->send(response);
                this->print(content, response->c_str());
            }
        } else {
            std::shared_ptr<const std::string> response = std::make_shared<const std::string>("422:{\"rules\":[\"type is required\",\"body is required\",\"body must be an object\"]}");
            this->send(response);
            this->print(content, response->c_str());
        }
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