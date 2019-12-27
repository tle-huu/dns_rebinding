#pragma once

#include <iostream>
#include <string>
#include <stdio.h>
#include <time.h>

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
inline const std::string current_date()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}


inline bool is_little_endian()
{
    static constexpr short int number = 0x1;
    const char *numPtr = reinterpret_cast<char const *>(&number);
    return (numPtr[0] == 1);
}
