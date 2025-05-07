#include "user.hpp"
#include "utils.hpp"
#include "authentication.hpp"
#include "storage.hpp"
#include "time.hpp"
#include "input.hpp"
#include <iostream>
#include <algorithm>

using std::string;

void User::display_menu(void)
{
    const int options = USER_OPT_COUNT - 1;
    std::string input;

    while (true)
    {
        Input::get("Menu options:\n"
                   "  1. View attendance logs\n  2. Logout\n"
                   "Enter the number of the option you want to perform: ",
                   input, NUM_MAX, options, Input::valid_opt);

        User_menu_opt selection = (User_menu_opt) std::stoi(input);

        switch (selection)
        {
        case VIEW:
            view_attendance();
            break;

        case LOGOUT:
            return;

        default:
            std::cerr << "Something has gone wrong, please try again later.\n";
            return;
        }
    }
}

void Manager::display_menu()
{
    const int options = MANAGER_OPT_COUNT - 1;
    std::string input;

    while (true)
    {
        Input::get("Manager Menu:\n"
                   "  1. View attendance logs\n  2. View analytics and reports\n"
                   "  3. Logout\n"
                   "Enter the number of the option you want to perform: ",
                   input, NUM_MAX, options, Input::valid_opt);

        Manager_menu_opt selection = (Manager_menu_opt) std::stoi(input);

        switch (selection)
        {
        case M_VIEW:
            view_attendance();
            break;

        case M_ANALYTICS:
            get_analytics();
            break;

        case M_LOGOUT:
            return;

        default:
            std::cerr << "Something has gone wrong, please try again later.\n";
            return;
        }
    }
}

void Admin::display_menu()
{
    const int options = ADMIN_OPT_COUNT - 1;
    std::string input;

    while (true)
    {
        Input::get("Admin Menu:\n"
                   "  1. View attendance logs\n  2. Edit attendance logs\n"
                   "  3. View analytics and reports\n  4. Assign/modify roles\n"
                   "  5. Logout\n"
                   "Enter the number of the option you want to perform: ",
                   input, NUM_MAX, options, Input::valid_opt);

        Admin_menu_opt selection = (Admin_menu_opt) std::stoi(input);

        switch (selection)
        {
        case A_VIEW:
            view_attendance();
            break;

        case EDIT:
            edit_attendance();
            break;

        case A_ANALYTICS:
            get_analytics();
            break;

        case ASSIGN_ROLE:
            assign_roles();
            break;

        case A_LOGOUT:
            return;

        default:
            std::cerr << "Something has gone wrong, please try again later.\n";
            return;
        }
    }
}

void User::view_attendance(void) const
{
    std::cout << "you are viewing the attendance.\n";
    return;
}

void Manager::view_attendance(void) const
{
    std::cout << "you are viewing the attendance.\n";
    return;
}

void Admin::view_attendance(void) const
{
    std::cout << "you are viewing the attendance.\n";
    return;
}

void Manager::get_analytics(void) const
{
    std::cout << "you are getting the analytics.\n";
    return;
}

void Admin::edit_attendance(void)
{
    std::cout << "you are editing the attendance.\n";
    return;
}

void Admin::assign_roles(void)
{
    std::cout << "you are assigning roles.\n";
    return;
}


void User::mark_attendance(void)
{
    auto today = Time::get_today_date();
    auto current_time = Time::get_current_time();

    auto it = std::find_if(attendance.begin(), attendance.end(),
        [this](const UserAttendance &user_att)
        {
            return user_att.user->get_username() == this->username;
        }
    );

    if (it == attendance.end())
    {
        std::cerr << "User has not been found in attendance container.\n";
        return;
    }

    DailyAttendance &log = it->logs[today];

    if (log.login_time.empty())
    {
        log.login_time = current_time;
        log.expected_login = Time::get_expected_login();
        log.expected_logout = Time::get_expected_logout();
        std::cout << "Login time marked: " << current_time << '\n';
    }
    else if (log.logout_time.empty())
    {
        log.logout_time = current_time;
        std::cout << "Logout time marked: " << current_time << '\n';
    }
    else
    {
        std::cout << "Attendance for today has already been filled.\n";
    }
}

bool User::logout(void)
{
    string confirmation;
    Input::get("Are you sure you want to logout? [y/n]: ", confirmation, NUM_MAX, Input::valid_yes_or_no);

    if (tolower(confirmation.at(0)) == 'n')
    {
        std::cout << "Logout cancelled, returning to menu.\n";
    }
    else
    {
        std::cout << "See you soon " << fullname.first << "!\n" 
                    << "Logging out...\n";

        mark_attendance();
        return true;
    }

    return false;
}

string User::get_role_name(void) const
{
    string role_str;

    switch (role)
    {
    case ADMIN:
        role_str = "Admin";
        break;
    
    case MANAGER:
        role_str = "Manager";
        break;
    
    case USER:
        role_str = "User";
        break;
    
    default:
        break;
    }

    return role_str;
}

bool UserAttendance::operator<(const UserAttendance &other) const
{
    return user->get_username() < other.user->get_username();
}

User *insert_into_attendance(std::unique_ptr<User> user)
{
    auto it = std::find_if(attendance.begin(), attendance.end(),
        [&](const UserAttendance &entry)
        {
            return entry.user && entry.user->get_username() == user->get_username();
        }
    );

    if (it != attendance.end())
    {
        return it->user.get();
    }

    UserAttendance record;
    record.user = std::move(user);
    attendance.push_back(std::move(record));

    return attendance.back().user.get();
}
