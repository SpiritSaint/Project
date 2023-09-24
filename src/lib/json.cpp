#include "json.h"

null_parser::null_parser() : p_(parse_options()) { }

std::size_t null_parser::write(const char *data, std::size_t size, boost::json::error_code &ec)  {
    auto const n = p_.write_some( false, data, size, ec );
    if(! ec && n < size)
        ec = error::extra_data;
    return n;
}

bool json_validator::check(string_view s) {
    null_parser p;
    error_code ec;
    p.write( s.data(), s.size(), ec );
    if( ec )
        return false;

    // The string is valid JSON.
    return true;
}
