#pragma once

#include <functional>
#include <unordered_map>
#include <vector>

static constexpr int RECORD_LIST_FIRST_SIZE = 20;

namespace dns
{

class Query;
class Response;
class Server;


/**
 *  Resolver is the class that handles the Query and resolves the domain
 *  names contained in it. It processes the Query and sets the appropriate
 *  values in the Response.
 */
class Resolver
{

private:

    using RecordHandle = std::function<bool(Resolver *, const Query&, Response&, std::string const &)>;

    enum RecordType : int
    {
        A = 1,
        PTR = 12,
        AAAA = 28,
    };


    const std::unordered_map<RecordType, RecordHandle> RECORDS_HANDLERS = {
        { A, &Resolver::a_record_handler},
        { PTR, &Resolver::ptr_record_handler},
        { AAAA, &Resolver::aaaa_record_handler},
    };

public:

    Resolver()
    {
        record_list_.reserve(RECORD_LIST_FIRST_SIZE);
    }

    /**
     *  Open the hosts file and read it to stores the ip_address-hostname pairs.
     */
    bool init(const std::string& filename,  std::string & error_output) noexcept;

    /**
     *  Process the query and sets the response to that query.
     */
    void process(const Query& query, Response& response, std::string const & client_ip) noexcept;

    void server(Server * server) { server_ = server; }

private:

    void store(const std::string& line) noexcept;

    /**
     *  Structure to hold the {ip_address, hostname} pairs.
     */
    struct Record
    {
        Record(std::string const & ip, std::string const & domain)
            : ip_address(ip)
            , domain_name(domain)
        {}

        Record() {}

        /**
         *  IP address in dot notation.
         */
        std::string ip_address;

        std::string domain_name;

    };

    std::vector<Record> record_list_;

    void print_records() noexcept;

    /**
     *  Convert IN-ADDR.ARPA domain to an IP addrress in dot notation
     *  reversing bytes.
     *
     *  Ex: 10.11.12.13.in-addr.arpa --> 13.12.11.10
     */
    std::string extract_ip(const std::string& domain);

    /**
     *  Finds in the list the domain corresponding to the ip address
     */
    const std::string find_domain_from_ip(const std::string& ip_address);

    /**
     *  Finds in the list the ip address corresponding to the domain
     */
    const std::string find_ip_from_domain(const std::string&);


// Records handlers
private:

    bool not_implemented(const Query&, Response&, std::string const &);
    bool a_record_handler(const Query& query, Response& response, std::string const & client_ip);
    bool aaaa_record_handler(const Query& query, Response& response, std::string const & client_ip);
    bool ptr_record_handler(const Query& query, Response& response, std::string const & client_ip);

private:

    Server * server_;

    Record target_record_;
};
}

