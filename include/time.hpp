#ifndef TIME_HPP
#define TIME_HPP

#include <string>

class Time
{
public:
    static std::string get_today_date(void); // displayed as yyyy-MM-dd 
    static std::string get_current_time(void); // displayed as HH:mm 
    static std::string get_expected_login(void);
    static std::string get_expected_logout(void);
};

#endif
