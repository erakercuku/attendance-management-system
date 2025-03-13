#include "storage.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

bool Storage::save_user(const User &user)
{
    std::ofstream file(USER_FILE, std::ios::app);
    if (!file)
    {
        std::cerr << "Error opening the file.\n";
        return false;
    }

    file << user.get_username() << ","
         << user.get_fullname().first << " " << user.get_fullname().second << ","
         << user.get_password() << ","
         << user.get_role() << "\n";

    file.close();
    return true;
}

bool Storage::load_user(const std::string &inp_username, Str_pair &fullname, std::string &password, Roles &role)
{
    std::ifstream file(USER_FILE);
    if (!file)
    {
        std::cerr << "Error opening the file.\n";
        return false;
    }

    std::string line, username, name, pass, role_str;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::getline(ss, username, ',');
        std::getline(ss, name, ',');
        std::getline(ss, pass, ',');
        std::getline(ss, role_str, ',');

        if (username == inp_username)
        {
            fullname = {name.substr(0, name.find(' ')), name.substr(name.find(' ') + 1)};
            password = pass;
            role = static_cast<Roles>(std::stoi(role_str));
            file.close();
            return true;
        }
    }

    file.close();
    return false;
}

bool Storage::save_attendance(const std::string &username, std::time_t timestamp)
{
    Attendance_vector logs;
    load_attendance(logs);

    std::ofstream file(ATTENDANCE_FILE, std::ios::trunc);
    if (!file)
    {
        std::cerr << "Error opening the file.\n";
        return false;
    }

    std::string time_str = time_to_str(timestamp);
    bool found = false;

    for (auto &[user, check_in, check_out] : logs)
    {
        if (user == username && check_out.empty())
        {
            if (!check_in.empty())
            {
                check_out = time_str;
            }
            else
            {
                check_in = time_str;
                check_out = "";
            }
            found = true;
        }
        file << user << "," << check_in << "," << check_out << "\n";
    }

    if (!found)
    {
        file << username << "," << time_str << ",\n";
    }

    file.close();
    return true;
}

bool Storage::load_attendance(Attendance_vector &attendance_log)
{
    std::ifstream file(ATTENDANCE_FILE);
    if (!file)
    {
        std::cerr << "Error opening the file.\n";
        return false;
    }

    std::string line, username, check_in, check_out;
    while (std::getline(file, line))
    {
        std::stringstream ss(line);
        std::getline(ss, username, ',');
        std::getline(ss, check_in, ',');
        std::getline(ss, check_out, ',');

        attendance_log.emplace_back(username, check_in, check_out);
    }

    file.close();
    return true;
}

std::string Storage::time_to_str(std::time_t timestamp)
{
    std::tm* time_info = std::localtime(&timestamp);
    std::ostringstream oss;
    oss << std::put_time(time_info, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
