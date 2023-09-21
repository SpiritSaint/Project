#include <iostream>
#include <boost/program_options.hpp>

#include "ext/dotenv.h"
#include "lib/configuration.h"

int main(int argc, char *argv[]) {
    boost::program_options::options_description program_options_description("Server");
    boost::program_options::variables_map program_options;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, program_options_description), program_options);
    boost::program_options::notify(program_options);

    auto _configuration = std::make_shared<configuration>();

    program_options_description.add_options()
            ("public_key", boost::program_options::value<std::string>(&_configuration->_keys._public), "Public key as PEM \n(string, default=public.pem)")
            ("private_key", boost::program_options::value<std::string>(&_configuration->_keys._public), "Private key as PEM \n(string, default=private.pem)");

    _configuration->init(program_options);

    dotenv::init(".env");
    return 0;
}
