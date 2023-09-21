#include <catch2/catch_test_macros.hpp>
#include "ext/dotenv.h"

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