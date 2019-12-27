#pragma once

#include <string>

#include "message.hpp"

namespace dns {

/**
 *  Class that represents the DNS Query and is able to code itself in its
 *  corresponding message format.
 */
class Query : public Message
{

public:

    Query() : Message(Message::QUERY) { }

    virtual ~Query() { }

    int code(char* buffer) noexcept;

    void decode(const char* buffer, int size) noexcept;

    std::string to_string() const noexcept;

    inline const std::string& name() const noexcept { return name_; }

    inline const uint type() const noexcept { return type_; }
    
    inline const uint query_class() const noexcept { return query_class_; }
    
private:
    std::string name_;
    uint type_;
    uint query_class_;

    void decode_qname(char const * & buffer) noexcept;
};
}

