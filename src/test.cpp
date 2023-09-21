#include <catch2/catch_test_macros.hpp>
#include "ext/dotenv.h"

TEST_CASE( "Basic", "[basic]" ) {
    REQUIRE( true == true );
    REQUIRE( false == false );
    REQUIRE( true != false );
}

TEST_CASE( "DotEnv", "[dotenv]" ) {
    dotenv::init(".env");

    REQUIRE( std::string("development").compare(std::getenv("APP_ENV")) == 0 );
    REQUIRE( std::string("production").compare(std::getenv("APP_ENV")) != 0 );
}