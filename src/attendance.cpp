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
    case ADMIN:
    {
        prompt += "  2. Edit attendance logs\n  3. View analytics and reports\n"
                  "  4. Assign/modify roles\n  5. Logout\n"
                  "Enter the number of the option you want to perform: ";
        options = ADMIN_OPT_COUNT - 21;

        while (true)
        {
            Input::get(prompt, input, NUM_MAX, options, Input::valid_opt);

            Admin_menu_opt selection = (Admin_menu_opt) (std::stoi(input) + 20);

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
                std::cout << "To be implemented.\n";
                break;
            
            case A_LOGOUT:
                if (logout(*this)) return;
                break;
            
            default:
                std::cerr << "Something has gone wrong, please try again later.\n";
                return;
            }
        }
        break;
    }
    case MANAGER:
    {
        prompt += "  2. View analytics and reports\n  3. Logout\n"
                  "Enter the number of the option you want to perform: ";
        options = MANAGER_OPT_COUNT - 11;

        while (true)
        {
            Input::get(prompt, input, NUM_MAX, options, Input::valid_opt);

            Manager_menu_opt selection = (Manager_menu_opt) (std::stoi(input) + 10);

            switch (selection)
            {
            case M_VIEW:
                view_attendance();
                break;
            
            case M_ANALYTICS:
                get_analytics();
                break;
            
            case M_LOGOUT:
                if (logout(*this)) return;
                break;
            
            default:
                std::cerr << "Something has gone wrong, please try again later.\n";
                return;
            }
        }
        break;
    }
    case USER:
    {
        prompt += "  2. Logout\n"
                  "Enter the number of the option you want to perform: ";

        while (true)
        {
            Input::get(prompt, input, NUM_MAX, options, Input::valid_opt);

            User_menu_opt selection = (User_menu_opt) std::stoi(input);

            switch (selection)
            {
            case VIEW:
                view_attendance();
                break;
            
            case LOGOUT:
                if (logout(*this)) return;
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
    Storage::save_attendance(user.get_username(), now);

    return;
}

void Attendance::view_attendance(void) const
{
    std::cout << "you are viewing the attendance.\n";
    return;
}

void Attendance::edit_attendance(void)
{
    std::cout << "you are editing the attendance.\n";
    return;
}

void Attendance::get_analytics(void) const
{
    std::cout << "you are getting the analytics.\n";
    return;
}

void Attendance::assign_roles(void)
{
    std::cout << "you are assigning roles.\n";
    return;
}
