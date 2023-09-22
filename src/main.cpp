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
    auto _state = std::make_shared<state>();

    program_description.add_options()
            ("public_key", boost::program_options::value<std::string>(&_configuration->_keys._public), "Public Key as PEM \n(string, default=public.pem)")
            ("private_key", boost::program_options::value<std::string>(&_configuration->_keys._public), "Private Key as PEM \n(string, default=private.pem)")
            ("dh_params", boost::program_options::value<std::string>(&_configuration->_keys._public), "DH Params as PEM \n(string, default=dh-params.pem)")
            ("env_file", boost::program_options::value<std::string>(&_configuration->_keys._public), "DotEnv file \n(string, default=.env)");

    _configuration->init(program_options);

    dotenv::init(_configuration->_env_file.c_str());

    boost::asio::ip::tcp::endpoint serve_endpoint { boost::asio::ip::tcp::v4(), _configuration->_ports._serve };
    boost::asio::io_context io_context;

    boost::asio::ssl::context secure_context { boost::asio::ssl::context::tlsv12 };
    secure_context.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::single_dh_use);
    secure_context.use_certificate_chain_file(_configuration->_keys._public);
    secure_context.use_private_key_file(_configuration->_keys._private, boost::asio::ssl::context::pem);
    secure_context.use_tmp_dh_file(_configuration->_keys._dh_params);

    boost::asio::signal_set signals(io_context, SIGINT, SIGTERM, SIGHUP);

    signals.async_wait([&](boost::system::error_code const &, int) {
        io_context.stop();
    });

    std::make_shared<http_listener>(io_context, secure_context, serve_endpoint, _state)->run();

    io_context.run();
    return 0;
}
