#include <catch2/catch_test_macros.hpp>
#include "ext/dotenv.h"
#include "lib/encryption.h"

TEST_CASE( "Basic", "[basic]" ) {
    REQUIRE( true == true );
    REQUIRE( false == false );
    REQUIRE( true != false );
}

TEST_CASE( "DotEnv", "[dotenv]" ) {
    dotenv::init(".env");

    std::string environment = std::getenv("APP_ENV");
    REQUIRE( environment == "development" );
    REQUIRE( environment != "production" );
}

TEST_CASE( "Encryption", "[encryption]" ) {
    auto config = std::make_shared<configuration>();

    config->_keys._public = "public.pem";
    config->_keys._private = "private.pem";

    auto encryptor = new encryption(config);

    std::string case_1 = "Lorem ipsum dolor.";
    std::string case_1_encrypted = encryptor->hide(case_1);
    std::string case_1_decrypted = encryptor->show(case_1_encrypted);

    std::string case_2 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Proin porta diam vel ultricies fermentum. Phasellus.";
    std::string case_2_encrypted = encryptor->hide(case_2);
    std::string case_2_decrypted = encryptor->show(case_2_encrypted);

    std::string case_3 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur tincidunt leo sit amet fermentum commodo. Vestibulum interdum, quam eu ullamcorper ornare, nisi sem porttitor nibh, eu pulvinar lorem dolor et justo. Nulla pellentesque nisi in est convallis aliquam. Praesent consequat neque in dolor fringilla sagittis. Nam ac pharetra enim. Proin pharetra lacinia ligula non commodo. Maecenas tincidunt nec odio id sagittis.";
    std::string case_3_encrypted = encryptor->hide(case_3);
    std::string case_3_decrypted = encryptor->show(case_3_encrypted);

    REQUIRE( case_1_decrypted == case_1 );
    REQUIRE( case_2_decrypted == case_2 );
    REQUIRE( case_3_decrypted == case_3 );
}