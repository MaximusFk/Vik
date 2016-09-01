#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <chrono>
#include <ctime>

using std::ofstream;

using std::chrono::system_clock;


namespace Log {

static bool i(const char * text)
{
    system_clock::time_point today = system_clock::now();
    time_t time = system_clock::to_time_t(today);
    ofstream file;
    file.open("./log.txt", std::ios_base::app | std::ios_base::out);
    if(!file.is_open())
        return false;
    file << std::ctime(&time) << '\t' << text << std::endl << std::endl;
    file.flush();
    file.close();
    return true;
}

static bool i(std::string str)
{
    return i(str.c_str());
}
}

#endif // LOG_H
