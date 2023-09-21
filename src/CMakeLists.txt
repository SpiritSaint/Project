cmake_minimum_required(VERSION 3.22.1)
project(app)

set(CMAKE_CXX_STANDARD 23)

find_package(Boost 1.83.0 REQUIRED COMPONENTS headers thread program_options json)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

file(GLOB_RECURSE BEAST_FILES ${Boost_INCLUDE_DIRS}/boost/beast/*.hpp ${Boost_INCLUDE_DIRS}/boost/beast/*.ipp)
source_group(TREE ${Boost_INCLUDE_DIRS}/boost/beast PREFIX beast FILES ${BEAST_FILES})

add_executable(app ext/mysql.cpp ${BEAST_FILES} main.cpp)
target_link_libraries(app PRIVATE Boost::headers Boost::json Boost::program_options Threads::Threads OpenSSL::Crypto OpenSSL::SSL)

target_compile_definitions(app PRIVATE BOOST_MYSQL_SEPARATE_COMPILATION)
