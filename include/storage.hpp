#ifndef STORAGE_HPP
#define STORAGE_HPP

#include "user.hpp"

class Storage
{
private:
    static constexpr const char* USER_FILE = "data/users.csv";
    static constexpr const char* ATTENDANCE_FILE = "data/attendance.csv";

public:
    static bool save_user(const User &user);
    static bool load_user(const std::string &inp_username, Str_pair &fullname, std::string &password, Roles &role);
    static bool save_attendance(const std::string &username, std::time_t timestamp);
    static bool load_attendance(Attendance_vector &attendance_logs);

    static std::string time_to_str(std::time_t timestamp);
};

#endif
