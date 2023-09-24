#ifndef JSON_H
#define JSON_H

#include "../ext/boost.h"

using namespace boost::json;

#include <iomanip>
#include <iostream>

class null_parser
{
    struct handler
    {
        constexpr static std::size_t max_object_size = std::size_t(-1);
        constexpr static std::size_t max_array_size = std::size_t(-1);
        constexpr static std::size_t max_key_size = std::size_t(-1);
        constexpr static std::size_t max_string_size = std::size_t(-1);

        bool on_document_begin( error_code& ) { return true; }
        bool on_document_end( error_code& ) { return true; }
        bool on_object_begin( error_code& ) { return true; }
        bool on_object_end( std::size_t, error_code& ) { return true; }
        bool on_array_begin( error_code& ) { return true; }
        bool on_array_end( std::size_t, error_code& ) { return true; }
        bool on_key_part( string_view, std::size_t, error_code& ) { return true; }
        bool on_key( string_view, std::size_t, error_code& ) { return true; }
        bool on_string_part( string_view, std::size_t, error_code& ) { return true; }
        bool on_string( string_view, std::size_t, error_code& ) { return true; }
        bool on_number_part( string_view, error_code& ) { return true; }
        bool on_int64( std::int64_t, string_view, error_code& ) { return true; }
        bool on_uint64( std::uint64_t, string_view, error_code& ) { return true; }
        bool on_double( double, string_view, error_code& ) { return true; }
        bool on_bool( bool, error_code& ) { return true; }
        bool on_null( error_code& ) { return true; }
        bool on_comment_part(string_view, error_code&) { return true; }
        bool on_comment(string_view, error_code&) { return true; }
    };

    basic_parser<handler> p_;

public:
    null_parser();
    ~null_parser()
    {
    }

    std::size_t write(char const* data, std::size_t size, error_code& ec);
};

class json_validator {
public:
    static bool check( string_view s );
};


#endif // JSON_H
