#include <catch2/catch_test_macros.hpp>
#include "ext/dotenv.h"

using namespace std;

TEST_CASE( "Basic", "[basic]" ) {
    REQUIRE( true == true );
    REQUIRE( false == false );
    REQUIRE( true != false );
}

TEST_CASE( "DotEnv", "[dotenv]" ) {
    dotenv::init(".env");

    REQUIRE( string{ std::getenv("APP_ENV") } == string { "development" } );
    REQUIRE( string{ std::getenv("APP_ENV") } != string { "production" } );
}