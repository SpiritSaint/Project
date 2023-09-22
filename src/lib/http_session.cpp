#include "http_session.h"

boost::beast::string_view mime_type(boost::beast::string_view path) {
    using boost::beast::iequals;
    auto const ext = [&path] {
        auto const pos = path.rfind(".");
        if(pos == boost::beast::string_view::npos)
            return boost::beast::string_view{};
        return path.substr(pos);
    }();
    if(iequals(ext, ".htm"))  return "text/html";
    if(iequals(ext, ".html")) return "text/html";
    if(iequals(ext, ".php"))  return "text/html";
    if(iequals(ext, ".css"))  return "text/css";
    if(iequals(ext, ".txt"))  return "text/plain";
    if(iequals(ext, ".js"))   return "application/javascript";
    if(iequals(ext, ".json")) return "application/json";
    if(iequals(ext, ".xml"))  return "application/xml";
    if(iequals(ext, ".swf"))  return "application/x-shockwave-flash";
    if(iequals(ext, ".flv"))  return "video/x-flv";
    if(iequals(ext, ".png"))  return "image/png";
    if(iequals(ext, ".jpe"))  return "image/jpeg";
    if(iequals(ext, ".jpeg")) return "image/jpeg";
    if(iequals(ext, ".jpg"))  return "image/jpeg";
    if(iequals(ext, ".gif"))  return "image/gif";
    if(iequals(ext, ".bmp"))  return "image/bmp";
    if(iequals(ext, ".ico"))  return "image/vnd.microsoft.icon";
    if(iequals(ext, ".tiff")) return "image/tiff";
    if(iequals(ext, ".tif"))  return "image/tiff";
    if(iequals(ext, ".svg"))  return "image/svg+xml";
    if(iequals(ext, ".svgz")) return "image/svg+xml";
    return "application/text";
}

std::string path_cat(boost::beast::string_view base, boost::beast::string_view path) {
    if(base.empty())
        return std::string(path);
    std::string result(base);
    char constexpr path_separator = '/';
    if(result.back() == path_separator)
        result.resize(result.size() - 1);
    result.append(path.data(), path.size());
    return result;
}

template <class Body, class Allocator> boost::beast::http::message_generator handle_request(
        std::shared_ptr<state> const & state,
        boost::beast::http::request<Body, boost::beast::http::basic_fields<Allocator>>&& req)
{
    auto const bad_request = [&] (boost::beast::string_view why) {
        boost::beast::http::response<boost::beast::http::string_body> res { boost::beast::http::status::bad_request, req.version() };
        res.set(boost::beast::http::field::server, state->_config->_serve.server_name.c_str());
        res.set(boost::beast::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = std::string(why);
        res.prepare_payload();
        return res;
    };

    auto const not_found = [&] (boost::beast::string_view target) {
        boost::beast::http::response<boost::beast::http::string_body> res { boost::beast::http::status::not_found, req.version() };
        res.set(boost::beast::http::field::server, state->_config->_serve.server_name.c_str());
        res.set(boost::beast::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "The resource '" + std::string(target) + "' was not found.";
        res.prepare_payload();
        return res;
    };

    auto const server_error = [&] (boost::beast::string_view what) {
        boost::beast::http::response<boost::beast::http::string_body> res { boost::beast::http::status::internal_server_error, req.version() };
        res.set(boost::beast::http::field::server, state->_config->_serve.server_name.c_str());
        res.set(boost::beast::http::field::content_type, "text/html");
        res.keep_alive(req.keep_alive());
        res.body() = "An error occurred: '" + std::string(what) + "'";
        res.prepare_payload();
        return res;
    };

    if( req.method() != boost::beast::http::verb::get && req.method() != boost::beast::http::verb::head )
        return bad_request("Unknown HTTP-method");

    if( req.target().empty() || req.target()[0] != '/' || req.target().find("..") != boost::beast::string_view::npos )
        return bad_request("Illegal request-target");

    std::string path = path_cat(state->_config->_serve._directory.c_str(), req.target());

    if(req.target().back() == '/')
        path.append("index.html");

    boost::beast::error_code ec;
    boost::beast::http::file_body::value_type body;
    body.open(path.c_str(), boost::beast::file_mode::scan, ec);

    if(ec == boost::beast::errc::no_such_file_or_directory)
        return not_found(req.target());

    if(ec)
        return server_error(ec.message());

    auto const size = body.size();

    if (req.method() == boost::beast::http::verb::head) {
        boost::beast::http::response<boost::beast::http::empty_body> res{boost::beast::http::status::ok, req.version()};
        res.set(boost::beast::http::field::server, state->_config->_serve.server_name.c_str());
        res.set(boost::beast::http::field::content_type, mime_type(path));
        res.set(boost::beast::http::field::access_control_allow_origin, "*");
        res.set(boost::beast::http::field::access_control_allow_methods, "HEAD");
        res.set(boost::beast::http::field::access_control_allow_headers, "*");
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return res;
    }

    boost::beast::http::response<boost::beast::http::file_body> res { std::piecewise_construct, std::make_tuple(std::move(body)), std::make_tuple(boost::beast::http::status::ok, req.version()) };
    res.set(boost::beast::http::field::server, state->_config->_serve.server_name.c_str());
    res.set(boost::beast::http::field::content_type, mime_type(path));
    res.set(boost::beast::http::field::access_control_allow_origin, "*");
    res.set(boost::beast::http::field::access_control_allow_methods, "GET, POST, OPTIONS, PUT, PATCH, DELETE");
    res.set(boost::beast::http::field::access_control_allow_headers, "*");
    res.content_length(size);
    res.keep_alive(req.keep_alive());
    return res;
}

http_session::http_session(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context & context, std::shared_ptr<state> state) :
    _stream(std::move(socket), context), _socket(std::move(socket)), _state(std::move(state)), _context(context) {
}

void http_session::run() {
    boost::beast::get_lowest_layer(_stream).expires_after(std::chrono::seconds(_state->_config->_serve.handshake_timeout));
    _stream.async_handshake(boost::asio::ssl::stream_base::server, boost::beast::bind_front_handler(& http_session::on_handshake, shared_from_this()));
}

void http_session::fail(boost::system::error_code error, char const * what) {
    if(error == boost::beast::net::ssl::error::stream_truncated) return;

    std::cout << "error::http_session: " << what << std::endl;
}

void http_session::on_read(boost::system::error_code error, std::size_t bytes) {
    boost::ignore_unused(bytes);

    if(error == boost::beast::http::error::end_of_stream)
        return do_close();

    if(error)
        return fail(error, "read");

    if(boost::beast::websocket::is_upgrade(_req)) {
        std::make_shared<websocket_session>(std::move(_stream), _state)->run(std::move(_req));
        return;
    }

    respond(handle_request(_state, std::move(_req)));
}

void http_session::on_write(bool keep_alive, boost::system::error_code error, std::size_t bytes)
{
    boost::ignore_unused(bytes);

    if (error)
        return fail(error, "write");

    if (!keep_alive) { return do_close(); }
    do_read();
}

void http_session::on_handshake(boost::system::error_code error) {
    if(error)
        return fail(error, "handshake");

    do_read();
}

void http_session::do_read() {
    _req = {};
    boost::beast::get_lowest_layer(_stream).expires_after(std::chrono::seconds(_state->_config->_serve.read_timeout));
    boost::beast::http::async_read(_stream, _buffer, _req, boost::beast::bind_front_handler(& http_session::on_read, shared_from_this()));
}

void http_session::do_close() {
    boost::system::error_code error;
    boost::beast::get_lowest_layer(_stream).expires_after(std::chrono::seconds(_state->_config->_serve.close_timeout));
    _stream.handshake(boost::asio::ssl::stream_base::client, error);
    _stream.async_shutdown(boost::beast::bind_front_handler(& http_session::on_shutdown, shared_from_this()));
}

void http_session::respond(boost::beast::http::message_generator && msg) {
    bool keep_alive = msg.keep_alive();
    boost::beast::async_write(_stream, std::move(msg), boost::beast::bind_front_handler(& http_session::on_write, this->shared_from_this(), keep_alive));
}

void http_session::on_shutdown(boost::system::error_code error) {
    if(error)
        return fail(error, "shutdown");
}