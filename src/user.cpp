#include "user.hpp"
#include "utils.hpp"
#include "attendance.hpp"
#include "authentication.hpp"
#include "storage.hpp"
#include "input.hpp"
#include <iostream>

using std::string;

bool User::login(void)
{
    string inp_username, inp_password;

    Input::get("Enter your username: ", inp_username, USERNAME_MAX, Input::valid_username);

    Input::get("Enter your password: ", inp_password, PASSWORD_MAX, Input::valid_password);

    Str_pair stored_fullname;
    string stored_password;
    Roles stored_role;

    if (Storage::load_user(inp_username, stored_fullname, stored_password, stored_role)
        && Auth::verify_password(inp_password, stored_password))
    {
        this->fullname.first = stored_fullname.first;
        this->fullname.second = stored_fullname.second;
        this->username = inp_username;
        this->password = stored_password;
        this->role = role;

        std::cout << "Welcome " << fullname.first << ".\n" 
                  << "You have successfully logged in with " << get_role_name() << " priviliges.\n";
        return true;
    }

    std::cout << "Invalid username or password.\n";
    return false;
}

void User::create_account(void)
{
    string inp_password, confirm_password, role_sel;
    const int options = ROLES_COUNT - 1;

    Input::get("Enter your first name: ", fullname.first, NAME_MAX, Input::valid_name);
    Input::get("Enter your last name: ", fullname.second, NAME_MAX, Input::valid_name);

    Input::get("Enter your username: ", username, USERNAME_MAX, Input::valid_username);

    Input::get("Enter your password: ", inp_password, PASSWORD_MAX, Input::valid_password);

    while (true)
    {
        std::cout << "Confirm your password: ";
        std::getline(std::cin, confirm_password);

        if (inp_password != confirm_password)
        {
            std::cout << "Please make sure the passwords match.\n";
        }
        else
        {
            std::cout << "Passwords match.\n";
            break;
        }
    }
    password = Auth::encrypt_password(inp_password);

    Input::get("Roles:\n  1. Administrator\n  2. Manager\n  3. User\n"
               "Enter the number of the role that will be assigned to you: ", 
               role_sel, NUM_MAX, options, Input::valid_opt);

    role = (Roles) std::stoi(role_sel);

    Storage::save_user(*this);

    std::cout << "Your account has been created.\n";
}

bool logout(Attendance &attendance)
{
    string confirmation;
    Input::get("Are you sure you want to logout? [y/n]: ", confirmation, NUM_MAX, Input::valid_yes_or_no);

    if (tolower(confirmation.at(0)) == 'n')
    {
        std::cout << "Logout cancelled, returning to menu.\n";
    }
    else
    {
        std::cout << "See you soon " << attendance.user.get_fullname().first << "!\n" 
                    << "Logging out...\n";

        attendance.mark_attendance();
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
