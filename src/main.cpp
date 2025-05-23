#include "utils.hpp"
#include "user.hpp"
#include "authentication.hpp"
#include "storage.hpp"
#include "input.hpp"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

using std::cout;
using std::cerr;
using std::cin;

AttendanceContainer attendance;
UserVector users;

int main(void)
{
    Login_opt selection;
    const int options = LOGIN_OPT_COUNT - 1;
    std::string input, confirmation;
    size_t login_attempts = 0;

    std::unique_ptr<User> user = nullptr;

    #ifdef CSV_VER
        Storage::load_all_users();
    #else
        Storage::load_users_from_db();
    #endif

    cout << "Welcome to the Attendance Management System.\n";

    do 
    {
        Input::get("What would you like to do?\n"
                   "  1. Login\n  2. Login with QR code\n  3. Create an account\n  4. Exit\n"
                   "Enter the number of the option you want to perform: ", 
                   input, NUM_MAX, options, Input::valid_opt);

        selection = (Login_opt) std::stoi(input);

        switch (selection) 
        {
        case LOGIN:
        {
            user = Auth::login();

            if (user)
            {
                User* raw_user = insert_into_attendance(std::move(user));

                #ifdef CSV_VER
                    Storage::load_attendance();
                #else
                    Storage::load_attendance_from_db();
                #endif

                raw_user->mark_attendance();
                raw_user->display_menu();

                login_attempts = 0;

                while (!raw_user->logout())
                {
                    raw_user->display_menu();
                }

                #ifdef CSV_VER
                    Storage::save_attendance();
                #else
                    Storage::save_attendance_to_db();
                #endif
            }
            else
            {
                login_attempts++;
                if (login_attempts < LOGIN_ATTEMPTS_MAX)
                {
                    cout << "Attempts remaining: " << (LOGIN_ATTEMPTS_MAX - login_attempts) << '\n';
                }
                else
                {
                    cout << "Too many failed login attempts. Please wait a few minutes before attempting again.";
                    std::this_thread::sleep_for(std::chrono::minutes(2));
                }
            }
            break;
        }
        case LOGIN_QR:
        {
            cout << "Camera module functionality is currently not available. Please login manually.\n";
            continue;

            user = Auth::login_with_qr();

            if (user)
            {
                User* raw_user = insert_into_attendance(std::move(user));

                #ifdef CSV_VER
                    Storage::load_attendance();
                #else
                    Storage::load_attendance_from_db();
                #endif

                raw_user->mark_attendance();
                raw_user->display_menu();

                login_attempts = 0;

                while (!raw_user->logout())
                {
                    raw_user->display_menu();
                }

                #ifdef CSV_VER
                    Storage::save_attendance();
                #else
                    Storage::save_attendance_to_db();
                #endif
            }
            else
            {
                login_attempts++;
                if (login_attempts < LOGIN_ATTEMPTS_MAX)
                {
                    cout << "Attempts remaining: " << (LOGIN_ATTEMPTS_MAX - login_attempts) << '\n';
                }
                else
                {
                    cout << "Too many failed login attempts. Please wait a few minutes before attempting again.";
                    std::this_thread::sleep_for(std::chrono::minutes(2));
                }
            }
            break;
        }
        case CREATE_ACCOUNT:
            user = Auth::create_account();
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
