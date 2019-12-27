#pragma once

#include "server.hpp"
#include "resolver.hpp"

namespace dns
{

class Application
{

public:

    Application() : server_(resolver_) { }


    /**
     *  Parse the port and hosts file from the arguments of main() function
     *
     *  If an error occurs, fills the error output parameter and returns false
     */
    bool parse_arguments(int const argc, char const * const * const argv, std::string & error) noexcept;

    bool run() noexcept;

private:

    int port_;
    std::string filename_;

    Resolver resolver_;
    Server server_;
};
}

