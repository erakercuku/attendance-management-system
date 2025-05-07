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
    static bool load_user(const std::string &inp_username, Str_pair &fullname, std::string &password, std::string &ID, Roles &role);
    static bool save_user_attendance(const UserAttendance &user_att);
    static bool load_user_attendance(UserAttendance &user_att);

    static bool save_all_users(void);
    static bool load_all_users(void);
    static bool save_attendance(void);
    static bool load_attendance(void);
};

#endif
