#include <iostream>
#include <sstream>


#include "message.hpp"
#include "response.hpp"
#include "query.hpp"

using namespace dns;

namespace dns
{

std::string Response::to_string() const noexcept
{

    std::ostringstream text;
    text << "\nRESPONSE { ";
    text << Message::to_string();

    text << "\tname: " << name_ << '\n';
    text << "\ttype: " << type_ << '\n';
    text << "\tclass: " << q_class_ << '\n';
    text << "\tttl: " << ttl_ << '\n';
    text << "\trdLength: " << rd_length_ << '\n';
    text << "\trdata: " << rdata_ << " }" << std::dec;

    return text.str();
}

void Response::decode(const char* buffer, int size) noexcept
{
    // Dont need this YET
}

int Response::code(char* buffer) noexcept
{
    char* bufferBegin = buffer;

    code_header(buffer);
    buffer += HDR_OFFSET;

    // Code Question section
    code_domain(buffer, name_);
    set_16_bits(buffer, type_);
    set_16_bits(buffer, q_class_);

    // Code Answer section

    // NAME
    code_domain(buffer, name_);
    // Type
    set_16_bits(buffer, type_);
    // Class
    set_16_bits(buffer, q_class_);
    // TTL
    set_32_bits(buffer, ttl_);
    // RDATA length
    set_16_bits(buffer, rd_length_);

    // RDATA

    if (type_ == 12)
    {
        code_domain(buffer, rdata_);
    }
    else
    {
        *buffer++ = rdata_.data()[0];
        *buffer++ = rdata_.data()[1];
        *buffer++ = rdata_.data()[2];
        *buffer++ = rdata_.data()[3];
    }
    
    int size = buffer - bufferBegin;

    return size;
}

void Response::code_domain(char*& buffer, const std::string& domain) noexcept
{

    int start = 0;
    int end = 0;

    while ((end = domain.find('.', start)) != std::string::npos)
    {

        *buffer++ = end - start;
        for (int i=start; i<end; i++)
        {

            *buffer++ = domain[i];
        }
        start = end + 1;
    }

    *buffer++ = domain.size() - start;
    for (int i=start; i<domain.size(); i++)
    {

        *buffer++ = domain[i];
    }

    *buffer++ = 0;
}

void Response::init_response_with_query(Query const & query) noexcept
{
    // Message specific
    id_ = ( query.id() );
    qd_count_ = 1;
    an_count_ = 1;

    // Response specific
    name_ = query.name();
    type_ = query.type();
    q_class_ = query.query_class();
    ttl_ = 2;
    rdata_ = "";
    rd_length_ = 1;
}


}
