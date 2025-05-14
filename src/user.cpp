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

void Manager::display_menu(void)
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

void Admin::display_menu(void)
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
    std::cout << "Your Attendance Log:\n";

    auto it = std::find_if(attendance.begin(), attendance.end(),
        [this](const UserAttendance &ua)
        {
            return ua.user->get_username() == this->username;
        });

    if (it == attendance.end())
    {
        std::cout << "No attendance record found.\n\n";
        return;
    }

    for (const auto &[date, log] : it->logs)
    {
        std::cout << "\nDate: " << date << '\n';
        std::cout << "  Login Time:     " << log.login_time << '\n';
        std::cout << "  Logout Time:    " << log.logout_time << '\n';
        std::cout << "  Expected Login: " << log.expected_login << '\n';
        std::cout << "  Expected Logout:" << log.expected_logout << '\n';
    }
    std::cout << '\n';
}

void Manager::view_attendance(void) const
{
    std::cout << "Viewing all users' attendance logs:\n";

    for (const auto &ua : attendance)
    {
        std::cout << "\nUser: " << ua.user->get_username() << '\n';

        for (const auto &[date, log] : ua.logs)
        {
            std::cout << "  Date: " << date << '\n';
            std::cout << "    Login Time:     " << log.login_time << '\n';
            std::cout << "    Logout Time:    " << log.logout_time << '\n';
            std::cout << "    Expected Login: " << log.expected_login << '\n';
            std::cout << "    Expected Logout:" << log.expected_logout << '\n';
        }
    }
    std::cout << '\n';
}

void Manager::get_analytics(void) const
{
    std::cout << "Linking to R analytics interface...\n";
    std::cout << "Opening external dashboard for attendance reports.\n";
}

void Admin::edit_attendance(void)
{
    std::string username;

    Input::get("Enter the username of the user to edit their attendance: ", username, USERNAME_MAX, Input::valid_username);

    auto it = std::find_if(attendance.begin(), attendance.end(),
        [&username](const UserAttendance &ua)
        {
            return ua.user->get_username() == username;
        });

    if (it == attendance.end())
    {
        std::cout << "User was not found in attendance records.\n";
        return;
    }

    std::string date;

    Input::get("Enter the date you want to edit (YYYY-MM-DD): ", date, DATE_MAX, Input::valid_date);

    auto log_it = it->logs.find(date);
    if (log_it == it->logs.end())
    {
        std::cout << "There is no attendance data for that date.\n";

        string confirmation;
        Input::get("Do you want to create attendance records for this date? [y/n]: ", confirmation, NUM_MAX, Input::valid_yes_or_no);

        if (tolower(confirmation.at(0)) == 'n')
        {
            std::cout << "New records cancelled, returning to menu.\n";
            return;
        }

        DailyAttendance new_log;
        new_log.expected_login = Time::get_expected_login();
        new_log.expected_logout = Time::get_expected_logout();

        it->logs[date] = new_log;
        log_it = it->logs.find(date);
    }

    DailyAttendance &log = log_it->second;

    std::string input;
    const int options = 5;

    Input::get("What would you like to edit?\n"
               "  1. Login Time\n  2. Logout Time\n  3. Expected Login\n"
               "  4. Expected Logout\n  5. Exit\n"
               "Enter the number of the option you want to perform: ", input, NUM_MAX, options, Input::valid_opt);
    
    int selection = std::stoi(input);

    if (selection == 5)
    {
        std::cout << "Returning to menu.\n";
        return;
    }

    std::string new_value;

    Input::get("Enter the new value (HH:MM): ", new_value, LOGIN_MAX, Input::valid_log_time);

    switch (selection)
    {
    case 1:
        log.login_time = new_value;
        break;

    case 2:
        log.logout_time = new_value;
        break;

    case 3:
        log.expected_login = new_value;
        break;

    case 4:
        log.expected_logout = new_value;
        break;

    default:
        std::cerr << "Something has gone wrong, please try again later.\n";
        break;
    }

    std::cout << "Attendance has been updated for " << username << " on " << date << ".\n";
}

void Admin::assign_roles(void)
{
    std::string username;

    Input::get("Enter the username of the user to edit their role: ", username, USERNAME_MAX, Input::valid_username);

    User *user = Storage::search_user_by_username(username);
    if (!user)
    {
        std::cout << "User has not been found.\n";
        return;
    }

    std::string input;
    Roles role;
    const int options = ROLES_COUNT - 1;

    Input::get("Choose the new role:\n"
               "  1. Admin\n  2. Manager\n  3. User\n"
               "Enter the number of the role you want to assign: ", 
               input, NUM_MAX, options, Input::valid_opt);
    
    role = (Roles) std::stoi(input);

    user->set_role(role);

    std::cout << user->get_role_name() << " privileges have been assigned to " << username << ".\n";
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
        return false;
    }

    std::cout << "See you soon " << fullname.first << "!\n" 
              << "Logging out...\n";

    mark_attendance();

    if (role == ADMIN)
    {
        if (!Storage::save_all_users())
        {
            std::cerr << "Failed to save users to file.\n";
        }
    }
    else
    {
        if (!Storage::save_user(*this))
        {
            std::cerr << "Failed to save user to file.\n";
        }
    }

    return true;
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
