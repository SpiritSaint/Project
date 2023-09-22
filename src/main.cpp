#include <iostream>

#include "ext/dotenv.h"
#include "ext/boost.h"
#include "lib/configuration.h"
#include "lib/http_listener.h"
#include "lib/state.h"

int main(int argc, char *argv[]) {
    boost::program_options::options_description program_description("Project");
    boost::program_options::variables_map program_options;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, program_description), program_options);
    boost::program_options::notify(program_options);

    auto _configuration = std::make_shared<configuration>();
    auto _state = std::make_shared<state>(_configuration);

    program_description.add_options()
            ("encryption_rsa_public_key", boost::program_options::value<std::string>(&_configuration->_rsa._public_key), "RSA Public key file \n(string, default=public.pem)")
            ("encryption_rsa_private_key", boost::program_options::value<std::string>(&_configuration->_rsa._private_key), "RSA Private key file \n(string, default=private.pem)")
            ("encryption_rsa_chained_key", boost::program_options::value<std::string>(&_configuration->_rsa._chained_key), "RSA Chained key file \n(string, default=chained.pem)")
            ("encryption_rsa_dh_params", boost::program_options::value<std::string>(&_configuration->_rsa._dh_params), "DH Parameters file \n(string, default=dh-params.pem)")
            ("encryption_rsa_size", boost::program_options::value<size_t>(&_configuration->_rsa._size), "RSA key size \n(number, default=2048)")
            ("environment", boost::program_options::value<std::string>(&_configuration->_environment), "Environment file \n(string, default=.env)")
            ("serve_port", boost::program_options::value<uint16_t>(&_configuration->_serve._port), "Serve port \n(number, default=443)")
            ("serve_directory", boost::program_options::value<std::string>(&_configuration->_serve._directory), "Serve directory \n(string, default=www)");

    _configuration->init(program_options);

    dotenv::init(_configuration->_environment.c_str());

    boost::asio::ip::tcp::endpoint serve_endpoint { boost::asio::ip::tcp::v4(), _configuration->_serve._port };
    boost::asio::io_context io_context;

    boost::asio::ssl::context secure_context { boost::asio::ssl::context::tlsv12 };
    secure_context.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::single_dh_use);
    secure_context.use_certificate_chain_file(_configuration->_rsa._chained_key);
    secure_context.use_private_key_file(_configuration->_rsa._private_key, boost::asio::ssl::context::pem);
    secure_context.use_tmp_dh_file(_configuration->_rsa._dh_params);

    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM, SIGHUP);

    signals.async_wait([&](boost::system::error_code const &, int) {
        io_context.stop();
    });

    std::make_shared<http_listener>(io_context, secure_context, serve_endpoint, _state)->run();

    io_context.run();
    return 0;
}
