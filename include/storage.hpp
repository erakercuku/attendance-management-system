#ifndef STORAGE_HPP
#define STORAGE_HPP

#include "user.hpp"

class Storage
{
private:
    static constexpr const char* USER_FILE = "data/users.csv";
    static constexpr const char* ATTENDANCE_FILE = "data/attendance.csv";
    static constexpr const char* HASHED_ID_FILE = "data/hashed_id.csv";

public:
    static void save_user_to_db(const User &user);
    static bool load_user_from_db(const std::string &username, Str_pair &fullname, std::string &password, std::string &ID, Roles &role);
    static void load_users_from_db(void);
    static void save_attendance_to_db(void);
    static void load_attendance_from_db(void);

    static bool save_user(const User &user);
    static bool load_user(const std::string &inp_username, Str_pair &fullname, std::string &password, std::string &ID, Roles &role);
    static bool save_user_attendance(const UserAttendance &user_att);
    static bool load_user_attendance(UserAttendance &user_att);

    static bool save_all_users(void);
    static bool load_all_users(void);
    static bool save_attendance(void);
    static bool load_attendance(void);

    static void save_qr_id_to_csv(const std::string &id);
};

#endif
