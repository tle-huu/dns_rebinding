#include <iostream>
#include <stdlib.h>

#include "application.hpp"


namespace dns
{

bool Application::parse_arguments(int argc, char const * const * const argv, std::string & error) noexcept
{

    if (argc != 3)
    {
        std::string text("Usage: dnsServer <port> <hostsFile>\n");
        text += "Example: dnsServer 4242 hosts\n";
        error = std::move(text);

        return false;
    }

    port_ = atoi(argv[1]);
    if (port_ < 1 || port_ > 65535) {
        
        std::string text("Error: Invalid port number.\n");
        error = std::move(text);
        return false;
    }

    filename_.assign(argv[2]);
    error = "";
    return true;
}

bool Application::run() noexcept
{
    std::string error;

    bool res = resolver_.init(filename_, error);
    if (res == false)
    {
        std::cerr << "Could not init resolver: " << error << '\n';
        return false;
    }

    res = server_.init(port_, error);

    if (res == false)
    {
        std::cerr << "Could not init server: " << error << '\n';
        return false;
    }
    server_.run();

    return true;
}

}
