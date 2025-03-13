#include "utils.hpp"
#include "user.hpp"
#include "attendance.hpp"
#include "input.hpp"
#include <iostream>

using std::cout;
using std::cerr;
using std::cin;

int main(void)
{
    Login_opt selection;
    const int options = LOGIN_OPT_COUNT - 1;
    std::string input, confirmation;

    User user;
    Attendance attendance(user);

    cout << "Welcome to the Attendance Management System.\n";

    do 
    {
        Input::get("What would you like to do?\n"
                   "  1. Login\n  2. Create an account\n  3. Exit\n"
                   "Enter the number of the option you want to perform: ", 
                   input, NUM_MAX, options, Input::valid_opt);

        selection = (Login_opt) std::stoi(input);

        switch (selection) 
        {
        case LOGIN:
        {            
            if (user.login())
            {
                attendance = Attendance(user);
                attendance.mark_attendance();
                attendance.display_menu();
            }
            break;
        }
        case CREATE_ACCOUNT:
            user.create_account();
            break;
        
        case EXIT:
            cout << "Goodbye, have a nice day!!\n";
            break;
        
        default:
            cerr << "Something has gone wrong, please try again later.\n";
            return 1;
        }
    } while (selection != EXIT);

    return 0;
}