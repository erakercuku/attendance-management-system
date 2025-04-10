#ifndef ATTENDANCE_HPP
#define ATTENDANCE_HPP

#include "user.hpp"
#include <map>

enum Admin_menu_opt
{
    A_VIEW = 21, 
    EDIT = 22, 
    A_ANALYTICS = 23, 
    ASSIGN_ROLE = 24, 
    A_LOGOUT = 25, 
    ADMIN_OPT_COUNT
};

enum Manager_menu_opt
{
    M_VIEW = 11, 
    M_ANALYTICS = 12, 
    M_LOGOUT = 13, 
    MANAGER_OPT_COUNT
};

enum User_menu_opt
{
    VIEW = 1, 
    LOGOUT = 2, 
    USER_OPT_COUNT
};

struct DailyAttendance
{
    std::string loginTime;
    std::string logoutTime;
    std::string expectedLogin;
    std::string expectedLogout;
};

// AttendanceRecord[date].DailyAttendance 
using AttendanceRecord = std::map<std::string, DailyAttendance>;

struct UserAttendance
{
    std::string username;
    AttendanceRecord records;
};

// AttendanceContainer[username].UserAttendance 
using AttendanceContainer = std::unordered_map<std::string, UserAttendance>;

class Attendance
{
private:
    User user;
    static std::map<std::string, Str_pair> logs;

public:
    explicit Attendance(const User &user) : user(user) {}

    void display_menu(void);
    void mark_attendance(void);
    void view_attendance(void) const;
    void edit_attendance(void);
    void get_analytics(void) const;
    void assign_roles(void);

    friend bool logout(Attendance &attendance);
};

#endif
