#pragma once

#include "message.hpp"

namespace dns
{

class Query;

/**
 *  Class that represents the DNS Response and is able to code itself in its
 *  corresponding message format.
 */
class Response : public Message
{

public:

    enum class Code : uint32_t
    {
        OK = 0,
        FORMAT_ERROR,
        SERVER_FAILURE,
        NAME_ERROR,
        NOT_IMPLEMENTED,
        REFUSED
    };

    Response() : Message(Message::RESPONSE) { }

    int code(char* buffer) noexcept;

    void decode(const char* buffer, int size) noexcept;

    void init_response_with_query(Query const & query) noexcept;

    inline void fill_name_error() noexcept
    {
        rcode_ = static_cast<std::underlying_type<Code>::type>(Response::Code::NAME_ERROR);
        rd_length_ = 1;
    }

    inline void fill_not_implemented() noexcept
    {
        rcode_ = static_cast<std::underlying_type<Code>::type>(Response::Code::NOT_IMPLEMENTED);
        rd_length_ = 1;
    }

    /**
     *  Returns the response message as a string text
     */
    std::string to_string() const noexcept;

    inline void rcode(Code code)                 noexcept { rcode_ = static_cast<std::underlying_type<Code>::type>(code); }
    inline void name(const std::string& value)   noexcept { name_ = value; }
    inline void type(const uint16_t value)       noexcept { type_ = value; }
    inline void q_class(const uint16_t value)    noexcept { q_class_ = value; }
    inline void ttl(const uint32_t value)        noexcept { ttl_ = value; }
    inline void rd_length(const uint16_t value)  noexcept { rd_length_ = value; }
    inline void rdata(std::string const & value) noexcept { rdata_ = value; }
    
    inline std::string const & r_data() const    noexcept { return rdata_; }
    
private:
    std::string name_;
    uint16_t    type_;
    uint16_t    q_class_;
    uint32_t    ttl_;
    uint16_t    rd_length_;
    std::string rdata_;

    void code_domain(char* & buffer, const std::string& domain) noexcept;
};
}

