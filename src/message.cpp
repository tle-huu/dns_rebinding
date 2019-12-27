#include <iostream>
#include <sstream>
#include <iomanip>
#include <netinet/in.h>


#include "message.hpp"
#include "utils.hpp"

namespace dns
{

// TODO URGENT: Need constexpr for magic values

std::string Message::to_string() const noexcept
{

    std::ostringstream text;
    text << "ID: " << std::showbase << std::hex << id_ << '\n' << std::noshowbase;
    text << "\tfields: [ QR: " << qr_ << " opCode: " << opcode_ << " ]" << '\n';
    text << "\tQDcount: " << qd_count_ << '\n';
    text << "\tANcount: " << an_count_ << '\n';
    text << "\tNScount: " << ns_count_ << '\n';
    text << "\tARcount: " << ar_count_ << '\n';

    return text.str();
}

void Message::decode_header(const char* buffer) noexcept
{
    id_ = read_16_bits(buffer);

    uint32_t fields = read_16_bits(buffer);
    qr_ = fields & QR_MASK;
    opcode_ = fields & OPCODE_MASK;
    aa_ = fields & AA_MASK;
    tc_ = fields & TC_MASK;
    rd_ = fields & RD_MASK;
    ra_ = fields & RA_MASK;

    qd_count_ = read_16_bits(buffer);
    an_count_ = read_16_bits(buffer);
    ns_count_ = read_16_bits(buffer);
    ar_count_ = read_16_bits(buffer);
}


// TODO: Finish this
void Message::code_header(char* buffer) noexcept
{
    set_16_bits(buffer, id_);

    int fields = (qr_ << 15);
    fields += (opcode_ << 14);
    fields += rcode_;
    set_16_bits(buffer, fields);

    set_16_bits(buffer, qd_count_);
    set_16_bits(buffer, an_count_);
    set_16_bits(buffer, ns_count_);
    set_16_bits(buffer, ar_count_);
}

int Message::read_16_bits(const char* & buffer) noexcept
{
    assert(is_little_endian());

    int value = static_cast<uint8_t> (buffer[0]);
    value = value << 8;
    value += static_cast<uint8_t> (buffer[1]);

    // Moving the pointer
    buffer += 2;

    return value;
}

void Message::set_16_bits(char*& buffer, uint16_t value) noexcept
{
    assert(is_little_endian());

    buffer[0] = (value & 0xFF00) >> 8;
    buffer[1] = value & 0xFF;

    // Moving the pointer
    buffer += 2;
}

void Message::set_32_bits(char*& buffer, uint32_t value) noexcept
{
    assert(is_little_endian());

    buffer[0] = (value & 0xFF000000) >> 24;
    buffer[1] = (value & 0xFF0000) >> 16;
    buffer[2] = (value & 0xFF00) >> 8;
    buffer[3] = (value & 0xFF);

    // Moving the pointer
    buffer += 4;
}

}
