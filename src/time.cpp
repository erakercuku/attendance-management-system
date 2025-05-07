#include "time.hpp"
#include <ctime>
#include <iomanip>
#include <sstream>

std::string Time::get_today_date(void)
{
    std::time_t t = std::time(nullptr);
    std::tm tm{};

    #ifdef _WIN32
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");

    return oss.str();
}

std::string Time::get_current_time(void)
{
    std::time_t t = std::time(nullptr);
    std::tm tm{};

    #ifdef _WIN32
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif

    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M");

    return oss.str();
}

std::string Time::get_expected_login(void)
{
    return "09:00";
}

std::string Time::get_expected_logout(void)
{
    return "17:00";
}
