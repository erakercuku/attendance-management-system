#include "attendance.hpp"
#include "utils.hpp"
#include "storage.hpp"
#include "input.hpp"
#include <iostream>
#include <fstream>
#include <ctime>

void Attendance::display_menu(void)
{
    int options = USER_OPT_COUNT - 1;
    std::string prompt, input;

    prompt = "Menu options:\n  1. View attendance logs\n";

    switch (user.get_role())
    {
    case 1: // ADMIN
    {
        prompt += "  2. Edit attendance logs\n  3. View analytics and reports\n"
                  "  4. Assign/modify roles\n  5. Logout\n"
                  "Enter the number of the option you want to perform: ";
        options = ADMIN_OPT_COUNT - 21;

        while (true)
        {
            Input::get(prompt, input, NUM_MAX, options, Input::valid_opt);

            Admin_menu_opt selection = (Admin_menu_opt)(std::stoi(input) + 20);

            switch (selection)
            {
            case A_VIEW:
                view_attendance();
                break;

            case EDIT:
                std::cout << "To be implemented.\n";
                break;

            case A_ANALYTICS:
                get_analytics();
                break;

            case ASSIGN_ROLE:
                assign_roles();
                break;

            case A_LOGOUT:
                if (logout(*this))
                    return;
                break;

            default:
                std::cerr << "Something has gone wrong, please try again later.\n";
                return;
            }
        }
        break;
    }
    case 2: // MANAGER
    {
        prompt += "  2. View analytics and reports\n  3. Logout\n"
                  "Enter the number of the option you want to perform: ";
        options = MANAGER_OPT_COUNT - 11;

        while (true)
        {
            Input::get(prompt, input, NUM_MAX, options, Input::valid_opt);

            Manager_menu_opt selection = (Manager_menu_opt)(std::stoi(input) + 10);

            switch (selection)
            {
            case M_VIEW:
                view_attendance();
                break;

            case M_ANALYTICS:
                get_analytics();
                break;

            case M_LOGOUT:
                if (logout(*this))
                    return;
                break;

            default:
                std::cerr << "Something has gone wrong, please try again later.\n";
                return;
            }
        }
        break;
    }
    case 3: // USER
    {
        prompt += "  2. Logout\n"
                  "Enter the number of the option you want to perform: ";

        while (true)
        {
            Input::get(prompt, input, NUM_MAX, options, Input::valid_opt);

            User_menu_opt selection = (User_menu_opt)std::stoi(input);

            switch (selection)
            {
            case VIEW:
                view_attendance();
                break;

            case LOGOUT:
                if (logout(*this))
                    return;
                break;

            default:
                std::cerr << "Something has gone wrong, please try again later.\n";
                return;
            }
        }
        break;
    }
    default:
        break;
    }
}

void Attendance::mark_attendance(void)
{
    std::time_t now = std::time(nullptr);
    if (!Storage::save_attendance(user.get_username(), now))
    {
        std::cerr << "Error: Unable to mark attendance.\n";
    }
    else
    {
        std::cout << "Attendance marked successfully for " << user.get_username() << ".\n";
    }
}


void Attendance::view_attendance(void) const
{
    std::string username = user.get_username();

    std::cout << "\n========== Attendance Logs ==========\n";

    switch (user.get_role())
    {
    case 1: // ADMIN
    {
        std::cout << "Admin access granted. Displaying all attendance logs:\n";
        Storage::print_all_attendance();
        break;
    }
    case 2: // MANAGER
    {
        std::cout << "Manager access granted. Displaying all attendance logs:\n";
        Storage::print_all_attendance();
        break;
    }
    case 3: // USER
    {
        std::cout << "User access granted. Displaying attendance logs for " << username << ":\n";
        Storage::print_user_attendance(username);
        break;
    }
    default:
    {
        std::cerr << "Error: Invalid user role detected.\n";
        break;
    }
    }

    std::cout << "=====================================\n";
}

void Attendance::edit_attendance(void)
{
    if (user.get_role() != 1) 
    {
        std::cerr << "Permission denied. Only admins can edit attendance records.\n";
        return;
    }
    std::cout << "You are editing the attendance.\n";
}


void Attendance::get_analytics(void) const
{
    std::cout << "You are getting the analytics.\n";
    return;
}

void log_role_change(const std::string &admin, const std::string &user, int old_role, int new_role)
{
    std::ofstream log_file("role_changes.log", std::ios::app);
    if (log_file)
    {
        std::time_t now = std::time(nullptr);
        log_file << "[" << std::ctime(&now) << "] Admin '" << admin << "' changed role of '" << user << "' from "
                 << role_to_string(old_role) << " to " << role_to_string(new_role) << "\n";
    }
}

std::string role_to_string(int role)
{
    switch (role)
    {
    case 1:
        return "Admin";
    case 2:
        return "Manager";
    case 3:
        return "User";
    default:
        return "Unknown";
    }
}

void Attendance::assign_roles(void)
{
    if (user.get_role() != 1) // Only Admin (Role 1) can assign roles
    {
        std::cerr << "Permission denied. Only admins can assign roles.\n";
        return;
    }

    std::string target_user, confirm;
    int new_role, duration = 0;

    std::cout << "\n========== Assign Roles ==========" << std::endl;
    Input::get("Enter the username of the user whose role you want to modify: ", target_user);

    if (!Storage::user_exists(target_user))
    {
