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
    config->_keys._size = 2048;

    auto encryptor = new encryption(config);

    std::string case_1 = "Lorem ipsum dolor.";
    std::string case_1_encrypted = encryptor->hide(case_1);
    std::string case_1_decrypted = encryptor->show(case_1_encrypted);

    std::string case_2 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Proin porta diam vel ultricies fermentum. Phasellus.";
    std::string case_2_encrypted = encryptor->hide(case_2);
    std::string case_2_decrypted = encryptor->show(case_2_encrypted);

    std::string case_3 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris utah aliquip consequat.";
    std::string case_3_encrypted = encryptor->hide(case_3);
    std::string case_3_decrypted = encryptor->show(case_3_encrypted);

    std::string case_4 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nullam tempus ornare faucibus. Etiam ac facilisis tortor, eu porttitor arcu. Ut eget imperdiet purus, et consectetur nisl. Fusce viverra dignissim purus, vitae facilisis dolor aliquet in. Ut eget tellus felis. Phasellus fringilla scelerisque risus, rutrum condimentum turpis vulputate ut. Ut dignissim eget libero et convallis. Donec porttitor non mi at gravida. Nullam pharetra ipsum nec auctor sagittis. Sed ultrices fermentum mauris vitae commodo biam.";
    std::string case_4_encrypted = encryptor->hide(case_4);
    std::string case_4_decrypted = encryptor->show(case_4_encrypted);

    REQUIRE( case_1_decrypted == case_1 );
    REQUIRE( case_2_decrypted == case_2 );
    REQUIRE( case_3_decrypted == case_3 );
    REQUIRE( case_4_decrypted == case_4 );
}