#pragma once

#include <chrono>
#include <netinet/in.h>
#include <unordered_map>

#include "query.hpp"
#include "response.hpp"

namespace dns {

class Resolver;

/**
 *  Server class is a socket server that receive queries and answer responses to
 *  those queries.
 */
class Server
{
private:
    using TIME_LAST_SEEN = uint64_t; // milliseconds since epoch

public:

    Server(Resolver & resolver);

    /**
     *  Initializes the server creating a UDP datagram socket and binding it to
     *  the INADDR_ANY address and the port passed.
     */
    bool init(int port, std::string & error_output) noexcept;

    /**
     *  The socket server runs in an infinite loop, waiting for queries and
     *  handling them through the Resolver and sending back the responses.
     */
    void run() noexcept;

    std::unordered_map<std::string, TIME_LAST_SEEN> const & clients() const
    {
        return clients_;
    }
    
private:

    static constexpr int BUFFER_SIZE = 1024;

    struct sockaddr_in address_;
    int sockfd_;

    Query query_;
    Response response_;

    Resolver& resolver_;

    std::unordered_map<std::string, TIME_LAST_SEEN> clients_;

};
}

