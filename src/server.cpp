#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <errno.h>


#include "server.hpp"
#include "resolver.hpp"
#include "utils.hpp"

namespace dns
{

Server::Server(Resolver& resolver) : resolver_(resolver)
{
    resolver_.server(this);
}

bool Server::init(int port, std::string & error_output) noexcept
{
    // Datagram socket
    sockfd_ = socket(AF_INET, SOCK_DGRAM, 0);

    address_.sin_family = AF_INET;

    // Default
    address_.sin_addr.s_addr = INADDR_ANY;
    address_.sin_port = htons(port);

    int res = bind(sockfd_, (struct sockaddr *) & address_,
                     sizeof (struct sockaddr_in));
    
    if (res != 0)
    {
        error_output = "Could not bind: " + std::string(strerror(errno));
        return false;
    }

    std::cout << "Listening in port: " << port << ", sockfd: " << sockfd_ << '\n';

    return true;
}

void Server::run() noexcept
{
    std::cout << "DNS Server running..." << '\n';

    char buffer[BUFFER_SIZE];
    struct sockaddr_in clientAddress;
    socklen_t addrLen = sizeof (struct sockaddr_in);

    while (true)
    {

        memset(buffer, 0, BUFFER_SIZE);
        int nbytes = recvfrom(sockfd_, buffer, BUFFER_SIZE, 0,
                     (struct sockaddr *) &clientAddress, &addrLen);

        char *client_ip = inet_ntoa(clientAddress.sin_addr);

        // clients_[client_ip] = std::chrono::duration_cast<std::chrono::milliseconds> \
        //                             (std::chrono::system_clock::now().time_since_epoch()).count();

        query_.decode(buffer, nbytes);
        

        // To not pollute the console with (millions) of the computer's other requests
        if (query_.name() != "mechant.mechant")
        {
            continue;
        }

        // Need to change this to a count of the query name
        ++clients_[client_ip];
        std::cout << "[" << current_date() << "] New request [" << clients_[client_ip] << "] of type [" << query_.type() << "] from  " << client_ip << " for " << query_.name() << '\n';

        resolver_.process(query_, response_, client_ip);

        // std::cout << response_.to_string() << '\n';

        memset(buffer, 0, BUFFER_SIZE);
        nbytes = response_.code(buffer);

        sendto(sockfd_, buffer, nbytes, 0, (struct sockaddr *) &clientAddress,
               addrLen);
    }
}

}
