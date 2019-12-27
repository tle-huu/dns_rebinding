#include <iostream>

#include "application.hpp"

/*
 *
 */
int main(int argc, char** argv)
{

    dns::Application application;

    std::string error;
    bool res = application.parse_arguments(argc, argv, error);

    if (res == false)
    {
        std::cerr << error << '\n';
        return 1;
    }
    application.run();

    return 0;
}
