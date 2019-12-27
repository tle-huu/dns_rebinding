#pragma once

#include <string>

namespace dns
{

/**
 *  Abstract class holfind, coding, and decoding DNS Messages
 *
 *  https://tools.ietf.org/html/rfc1035
 */
class Message
{

public:

    enum Type
    {
        QUERY = 0,
        RESPONSE,
    };

    virtual int code(char* buffer) noexcept = 0;

    virtual void decode(const char* buffer, int size) noexcept = 0;

    inline uint32_t id()       const     noexcept { return id_;       }
    inline uint32_t qd_count() const     noexcept { return qd_count_; }
    inline uint32_t an_count() const     noexcept { return an_count_; }
    inline uint32_t ns_count() const     noexcept { return ns_count_; }
    inline uint32_t ar_count() const     noexcept { return ar_count_; }

    inline void id(uint32_t id)          noexcept { id_ = id;          }
    inline void qd_count(uint32_t count) noexcept { qd_count_ = count; }
    inline void an_count(uint32_t count) noexcept { an_count_ = count; }
    inline void ns_count(uint32_t count) noexcept { ns_count_ = count; }
    inline void ar_count(uint32_t count) noexcept { ar_count_ = count; }

protected:
    static constexpr uint32_t HDR_OFFSET = 12;

    uint32_t id_;
    uint32_t qr_;
    uint32_t opcode_;
    uint32_t aa_;
    uint32_t tc_;
    uint32_t rd_;
    uint32_t ra_;
    uint32_t rcode_;
    
    uint32_t qd_count_;
    uint32_t an_count_;
    uint32_t ns_count_;
    uint32_t ar_count_;

    Message(Type type) : qr_(type) {}

    virtual ~Message() {}

    virtual std::string to_string() const noexcept;

    void decode_header(const char* buffer) noexcept;

    void code_header(char* buffer) noexcept;

    int read_16_bits(const char*& buffer) noexcept;

    void set_16_bits(char*& buffer, uint16_t value) noexcept;

    void set_32_bits(char*& buffer, uint32_t value) noexcept;

private:
    static constexpr uint32_t QR_MASK       = 0x8000;
    static constexpr uint32_t OPCODE_MASK   = 0x7800;
    static constexpr uint32_t AA_MASK       = 0x0400;
    static constexpr uint32_t TC_MASK       = 0x0200;
    static constexpr uint32_t RD_MASK       = 0x0100;
    static constexpr uint32_t RA_MASK       = 0x8000;
    static constexpr uint32_t RCODE_MASK    = 0x000F;
};
}

