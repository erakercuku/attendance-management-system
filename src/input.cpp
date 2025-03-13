#include "input.hpp"
#include "utils.hpp"
#include "user.hpp"
#include <iostream>
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
    // add other conditions for minimum username length, restriction of characters, etc. 

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
    // add other conditions for minimum password length, inclusion and restriction of characters, password strength, etc. 

    return true;
}


bool Input::valid_role(const std::string &role)
{
    if (role.empty())
    {
        cerr << "Invalid role. role can not be blank, please enter a valid role.\n";
        return false;
    }
    if (isspace((unsigned char) role.at(0)))
    {
        cerr << "Invalid role. role can not start with a space, please enter a valid role.\n";
        return false;
    }
    // add other conditions for specific role selection, etc. 

    return true;
}
