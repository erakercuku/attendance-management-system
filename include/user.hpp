#ifndef USER_HPP
#define USER_HPP

#include "utils.hpp"
#include <memory>
#include <map>
#include <vector>

enum Roles {ADMIN = 1, MANAGER, USER, ROLES_COUNT};

enum Login_opt
{
    LOGIN = 1,
    LOGIN_QR = 2, 
    CREATE_ACCOUNT = 3,
    EXIT = 4,
    LOGIN_OPT_COUNT
};

enum User_menu_opt
{
    VIEW = 1, 
    QR_CODE = 2, 
    LOGOUT = 3, 
    USER_OPT_COUNT
};

enum Manager_menu_opt
{
    M_VIEW = 1, 
    M_ANALYTICS = 2, 
    M_QR_CODE = 3, 
    M_LOGOUT = 4, 
    MANAGER_OPT_COUNT
};

enum Admin_menu_opt
{
    A_VIEW = 1, 
    EDIT = 2, 
    A_ANALYTICS = 3, 
    ASSIGN_ROLE = 4, 
    A_QR_CODE = 5, 
    A_LOGOUT = 6, 
    ADMIN_OPT_COUNT
};

// User, Manager, & Admin classes related code 

class User
{
protected:
    Str_pair fullname;
    std::string username;
    std::string password;
    std::string ID;
    Roles role;

public:
    User(void) = default;
    User(const Str_pair &fullname, const std::string &username, 
         const std::string &password, const std::string &ID, const Roles role)
        : fullname(fullname), username(username), password(password), ID(ID), role(role) {}

    virtual ~User() = default;

    virtual void display_menu(void);
    virtual void view_attendance(void) const;
    void mark_attendance(void);
    bool logout(void);

    Str_pair get_fullname(void) const { return fullname; }
    std::string get_username(void) const { return username; }
    std::string get_password(void) const { return password; }
    std::string get_ID(void) const { return ID; }
    Roles get_role(void) const { return role; }
    std::string get_role_name(void) const;

    // void set_fullname(const Str_pair fullname) { this->fullname = fullname; }
    // void set_username(const std::string &username) { this->username = username; }
    // void set_password(const std::string &password) { this->password = password; }
    // void set_ID(const std::string &ID) { this->ID = ID; }
    void set_role(Roles role) { this->role = role; }
};

class Manager : public User
{    
public:
    using User::User;

    void display_menu(void) override;
    void view_attendance(void) const override;
    void get_analytics(void) const;
};

class Admin : public Manager
{ 
public:
    using Manager::Manager;

    void display_menu(void) override;
    void edit_attendance(void);
    void assign_roles(void);
};

// Attendance container related code 

struct DailyAttendance
{
    std::string login_time;
    std::string logout_time;
    std::string expected_login;
    std::string expected_logout;
};

// AttendanceRecord[date].DailyAttendance 
using AttendanceRecord = std::map<std::string, DailyAttendance>;

struct UserAttendance
{
    std::unique_ptr<User> user;
    AttendanceRecord logs;

    bool operator<(const UserAttendance &other) const;
};

using AttendanceContainer = std::vector<UserAttendance>;

extern AttendanceContainer attendance;

using UserVector = std::vector<std::unique_ptr<User>>;

extern UserVector users;

User *insert_into_attendance(std::unique_ptr<User> user);

#endif
