#include <iostream>
#include <sstream>


#include "query.hpp"


namespace dns
{

std::string Query::to_string() const noexcept
{

    std::ostringstream text;
    text << "\nQUERY { ";
    text << Message::to_string();
    text << "\tname: " << name_ << '\n';
    text << "\ttype: " << type_ << '\n';
    text << "\tclass: " << query_class_ << '\n';
    text << " }" << std::dec;

    return text.str();
}

// Is this is to be used anyday soon ?
int Query::code(char* buffer) noexcept
{
    return 0;
}

void Query::decode(const char* buffer, int size) noexcept
{
    decode_header(buffer);
    buffer += HDR_OFFSET;
    
    decode_qname(buffer);

    type_ = read_16_bits(buffer);
    query_class_ = read_16_bits(buffer);
}

void Query::decode_qname(char const * & buffer) noexcept
{
    name_.clear();

    int length = *buffer;

    ++buffer;
    while (length != 0)
    {
        name_.append(buffer, length);
        buffer += length;

        length = *buffer;
        ++buffer;

        if (length != 0)
        {
            name_.append(1,'.');
        }
    }
}

}
