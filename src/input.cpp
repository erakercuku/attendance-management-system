#include "input.hpp"
#include "utils.hpp"
#include "user.hpp"
#include "storage.hpp"
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cctype>

using std::cout;
using std::cerr;
using std::cin;
using std::getline;
using std::endl;

/* handles input processing from the user 
&prompt: case-dependent const string reference that asks the user to enter an input 
&input: reference to the string that is going to store the input 
size: size_t type number that stores the maximum allowed size of the string input 
*validate: function pointer that calls the specific function used to validate the 
    input. it carries a const string reference as a parameter, and it returns a bool 
it has no return type and only calls upon other bool functions */
void Input::get(const std::string &prompt, std::string &input, size_t size, 
                bool (*validate)(const std::string &))
{
    bool valid = false;

    while (!valid) 
    {
        cout << prompt;
        if (!getline(cin, input)) 
        {
            cerr << "Something has gone wrong reading the input." << endl;
            cin.clear();
            CLEAR_INPUT_BUFFER;
            input.clear();
            continue;
        }

        if (input.length() >= size) 
        {
            cerr << "Invalid input. Input may not exceed " << size - 1 << " characters." << endl;
            input.clear();
            continue;
        }

        #ifdef CSV_VER
            bool has_comma = false;

            for (char c : input)
            {
                if (c == ',')
                {
                    cerr << "Invalid input. Input may not contain a comma.\n";
                    has_comma = true;
                    break;
                }
            }

            if (has_comma) continue;
        #endif

        while (!input.empty() && isspace((unsigned char) input.back()))
        {
            input.pop_back();
        }

        valid = validate(input);
    }
}

/* handles input processing for option menus from the user 
additional parameters: 
opt_num: const integer that stores the number of options available 
*validate: function pointer that calls the specific function used to validate the 
    input. it carries a const string reference and a const integer as parameters, 
    and it returns a bool 
it has no return type and only calls upon other bool functions */
void Input::get(const std::string &prompt, std::string &input, size_t size, 
                const int opt_num, bool (*validate)(const std::string &, const int))
{
    bool valid = false;

    while (!valid) 
    {
        cout << prompt;
        if (!getline(cin, input)) 
        {
            cerr << "Something has gone wrong when reading the input." << endl;
            cin.clear();
            CLEAR_INPUT_BUFFER;
            input.clear();
            continue;
        }

        if (input.length() >= size - 1) 
        {
            cerr << "Invalid input. Input may not exceed " << size - 1 << " characters." << endl;
            input.clear();
            continue;
        }

        while (!input.empty() && isspace((unsigned char) input.back()))
        {
            input.pop_back();
        }

        valid = validate(input, opt_num);
    }
}

/* validates the input for the menu options 
&inp: const string reference that stores the input from the user 
opt_num: const integer that stores the number of options available 
it returns false if the input does not meet the requirements, and it returns true 
if it does */
bool Input::valid_opt(const std::string &inp, const int opt_num)
{
    if (inp.length() > 1)
    {
        cerr << "Invalid input. Please enter a single valid number.\n";
        return false;
    }
    if (inp.empty())
    {
        cerr << "Invalid input. Input can not be blank, please enter a single valid number.\n";
        return false;
    }

    int num = 0;
    try 
    {
        num = std::stoi(inp);
    }
    catch (const std::invalid_argument &e)
    {
        cerr << "Invalid input. Please enter a valid integer.\n";
        return false;
    }

    if (num < 1 || num > opt_num) 
    {
        cerr << "Invalid input. Please enter a valid number from the given options.\n";
        return false;
    } 

    return true;
}

/* validates the input for a confirmation [y/n] 
&inp: const string reference that stores the input from the user 
it returns false if the input does not meet the requirements, and it returns true 
if it does */
bool Input::valid_yes_or_no(const std::string &inp)
{
    if (inp.length() > 1)
    {
        cerr << "Invalid input. Please enter either 'y' or 'n'.\n";
        return false;
    }
    if (inp.empty())
    {
        cerr << "Invalid input. Input can not be blank, please enter either 'y' or 'n'.\n";
        return false;
    }
    if (tolower(inp.at(0)) != 'y' && tolower(inp.at(0)) != 'n')
    {
        cerr << "Invalid input. Please enter either 'y' or 'n'.\n";
        return false;
    }

    return true;
}

/* validates the name of the user
&name: const string reference that stores the input name from the user 
it returns false if the input does not meet the name requirements, and it returns 
true if it does */
bool Input::valid_name(const std::string &name)
{
    if (name.empty())
    {
        cerr << "Invalid name. Name can not be blank, please enter a valid name.\n";
        return false;
    }
    if (isspace((unsigned char) name.at(0)))
    {
        cerr << "Invalid name. Name can not start with a space, please enter a valid name.\n";
        return false;
    }

    return true;
}


bool Input::valid_username(const std::string &username)
{
    if (username.empty())
    {
        cerr << "Invalid username. Username can not be blank, please enter a valid username.\n";
        return false;
    }
    if (isspace((unsigned char) username.at(0)))
    {
        cerr << "Invalid username. Username can not start with a space, please enter a valid username.\n";
        return false;
    }
    if (username.length() < USERNAME_MIN)
    {
        cerr << "Invalid username. Username must be at least " << USERNAME_MIN << " characters long.\n";
        return false;
    }

    for (char c : username)
    {
        if (static_cast<unsigned char>(c) > 127)
        {
            cerr << "Invalid username. Username contains invalid characters, please enter a valid username.\n";
            return false;
        }
    }

    return true;
}

bool Input::valid_password(const std::string &password)
{
    if (password.empty())
    {
        cerr << "Invalid password. Password can not be blank, please enter a valid password.\n";
        return false;
    }
    if (isspace((unsigned char) password.at(0)))
    {
        cerr << "Invalid password. Password can not start with a space, please enter a valid password.\n";
        return false;
    }
    if (password.length() < PASSWORD_MIN)
    {
        cerr << "Invalid password. Password must be at least " << PASSWORD_MIN << " characters long.\n";
        return false;
    }

    return true;
}

bool Input::valid_password_create(const std::string &password)
{
    if (!valid_password(password)) return false;

    bool has_upper = false, has_lower = false, has_digit = false, has_special = false;

    for (char c : password)
    {
        if (static_cast<unsigned char>(c) > 127)
        {
            cerr << "Invalid password. Password contains invalid characters, please enter a valid password.\n";
            return false;
        }
        if (std::isupper(c)) has_upper = true;
        else if (std::islower(c)) has_lower = true;
        else if (std::isdigit(c)) has_digit = true;
        else if (std::ispunct(c)) has_special = true;
    }

    cout << "Password strength: ";
    int strength = has_upper + has_lower + has_digit + has_special;

    switch (strength)
    {
    case 4:
        cout << "Strong\n";
        break;

    case 3:
        cout << "Moderate\n";
        break;

    case 2:
        cout << "Weak\n";
        break;

    default:
        cout << "Very Weak\n";
        break;
    }

    if (!has_upper) cerr << "Password must contain at least one uppercase letter.\n";
    if (!has_lower) cerr << "Password must contain at least one lowercase letter.\n";
    if (!has_digit) cerr << "Password must contain at least one number.\n";
    if (!has_special) cerr << "Password must contain at least one special character.\n";

    return (has_upper && has_lower && has_digit && has_special);
}

bool Input::valid_date(const std::string &date)
{
    if (date.size() != 10 || date[4] != '-' || date[7] != '-')
    {
        cerr << "Invalid date format. Please use the valid format: YYYY-MM-DD.\n";
        return false;
    }

    std::string year_val = date.substr(0, 4);
    std::string month_val = date.substr(5, 2);
    std::string day_val = date.substr(8, 2);

    if (!std::all_of(year_val.begin(), year_val.end(), ::isdigit) || 
        !std::all_of(month_val.begin(), month_val.end(), ::isdigit) || 
        !std::all_of(day_val.begin(), day_val.end(), ::isdigit))
    {
        cerr << "Invalid date values. Please enter the year (YYYY), month (MM), and day (DD) as numbers.\n";
        return false;
    }

    int year = std::stoi(year_val);
    int month = std::stoi(month_val);
    int day = std::stoi(day_val);

    if (month < JAN || month > DEC)
    {
        cerr << "Invalid month. Please enter a valid month (01-12).\n";
        return false;
    }
    if (day < 1 || day > 31)
    {
        cerr << "Invalid day. Please enter a valid day (01-31).\n";
        return false;
    }
    if ((month == APR || month == JUN || month == SEP || month == NOV) && day > 30)
    {
        cerr << "Invalid day. The month you entered does not have " << day << " days, please enter a valid date.\n";
        return false;
    }
    if (month == FEB && (day > 29 || (day == 29 && !IS_LEAP_YEAR(year))))
    {
        cerr << "Invalid day. February does not have " << day << " days"
             << ((day == 29)? " in the year you entered" : "") << ", please enter a valid date.\n";
        return false;
    }

    return true;
}

bool Input::valid_log_time(const std::string &log_time)
{
    if (log_time.size() != 5 || log_time[2] != ':')
    {
        cerr << "Invalid log time format. Please use the valid format: HH:MM.\n";
        return false;
    }

    std::string hour_val = log_time.substr(0, 2);
    std::string minute_val = log_time.substr(3, 2);

    if (!std::all_of(hour_val.begin(), hour_val.end(), ::isdigit) || 
        !std::all_of(minute_val.begin(), minute_val.end(), ::isdigit))
    {
        cerr << "Invalid log time values. Please enter the hours (HH) and minutes (MM) as numbers.\n";
        return false;
    }

    int hour = std::stoi(hour_val);
    int minute = std::stoi(minute_val);

    if (hour < 0 || hour > 23)
    {
        cerr << "Invalid log time hour value. Please enter a valid hour value (00-23).\n";
        return false;
    }
    if (minute < 0 || minute > 59)
    {
        cerr << "Invalid log time minute value. Please enter a valid minute value (00-59).\n";
        return false;
    }

    return true;
}

bool Input::is_username_unique(const std::string &username)
{
    for (const auto &user : users)
    {
        if (user->get_username() == username) return false;
    }

    return true;
}
