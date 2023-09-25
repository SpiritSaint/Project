cmake_minimum_required(VERSION 3.22.1)
project(app)

set(CMAKE_CXX_STANDARD 20)
set(LIBRARIES Boost::headers Boost::json Boost::program_options Threads::Threads OpenSSL::Crypto OpenSSL::SSL)
set(SOURCE
        ext/mysql.cpp
        ext/dotenv.h
        ext/boost.h
        ext/std.h
        lib/state.h
        lib/exceptions.h
        lib/configuration.h
        lib/encryption.cpp
        lib/encryption.h
        lib/http_listener.cpp
        lib/http_listener.h
        lib/http_session.cpp
        lib/http_session.h
        lib/websocket_session.cpp
        lib/websocket_session.h
        ext/base64.h
        lib/session.cpp
        lib/session.h
        lib/json.cpp
        lib/json.h)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pthread" )

find_package(Boost 1.83.0 REQUIRED COMPONENTS headers thread program_options json)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
)
FetchContent_MakeAvailable(Catch2)

file(GLOB_RECURSE BEAST_FILES ${Boost_INCLUDE_DIRS}/boost/beast/*.hpp ${Boost_INCLUDE_DIRS}/boost/beast/*.ipp)
source_group(TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})

add_executable(app ${SOURCE} ${BEAST_FILES} main.cpp)
target_link_libraries(app PRIVATE ${LIBRARIES})
target_compile_definitions(app PRIVATE BOOST_MYSQL_SEPARATE_COMPILATION)

add_executable(test ${SOURCE} ${BEAST_FILES} test.cpp)
target_link_libraries(test PRIVATE ${LIBRARIES} Catch2::Catch2WithMain)
target_compile_definitions(test PRIVATE BOOST_MYSQL_SEPARATE_COMPILATION)