#include "storage.hpp"
#include "authentication.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

bool Storage::save_user(const User &user)
{
    std::ofstream file(USER_FILE, std::ios::app);
    if (!file) return false;

    file << user.get_fullname().first << ',' << user.get_fullname().second << ','
         << user.get_username() << ',' << user.get_password() << ','
         << user.get_ID() << ',' << static_cast<int>(user.get_role()) << '\n';

    return true;
}

bool Storage::load_user(const std::string &username, Str_pair &fullname, std::string &password,
                        std::string &ID, Roles &role)
{
    std::ifstream file(USER_FILE);
    if (!file) return false;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string fname, lname, uname, pass, id, role_str;

        if (!std::getline(ss, fname, ',')) continue;
        if (!std::getline(ss, lname, ',')) continue;
        if (!std::getline(ss, uname, ',')) continue;
        if (!std::getline(ss, pass, ',')) continue;
        if (!std::getline(ss, id, ',')) continue;
        if (!std::getline(ss, role_str)) continue;

        if (uname == username)
        {
            fullname = {fname, lname};
            password = pass;
            ID = id;
            role = static_cast<Roles>(std::stoi(role_str));
            return true;
        }
    }

    return false;
}

bool Storage::save_user_attendance(const UserAttendance &user_att)
{
    std::ofstream file(ATTENDANCE_FILE, std::ios::app);
    if (!file) return false;

    const auto &uname = user_att.user->get_username();
    const auto &logs = user_att.logs;

    for (const auto &[date, entry] : logs)
    {
        file << uname << ',' << date << ',' 
             << entry.login_time << ',' << entry.logout_time << ',' 
             << entry.expected_login << ',' << entry.expected_logout << '\n';
    }

    return true;
}

bool Storage::load_user_attendance(UserAttendance &user_att)
{
    std::ifstream file(ATTENDANCE_FILE);
    if (!file) return false;

    std::string line;
    std::string uname = user_att.user->get_username();

    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string username, date, in_time, out_time, expected_in, expected_out;

        if (!std::getline(ss, username, ',')) continue;
        if (!std::getline(ss, date, ',')) continue;
        if (!std::getline(ss, in_time, ',')) continue;
        if (!std::getline(ss, out_time, ',')) continue;
        if (!std::getline(ss, expected_in, ',')) continue;
        if (!std::getline(ss, expected_out)) continue;

        if (username == uname)
        {
            user_att.logs[date] = {in_time, out_time, expected_in, expected_out};
        }
    }

    return true;
}

bool Storage::save_all_users(void)
{
    std::ofstream file(USER_FILE);
    if (!file) return false;

    for (const auto &user_att : attendance)
    {
        const User &user = *user_att.user;

        file << user.get_fullname().first << ',' << user.get_fullname().second << ','
             << user.get_username() << ',' << user.get_password() << ','
             << user.get_ID() << ',' << static_cast<int>(user.get_role()) << '\n';
    }

    return true;
}

bool Storage::load_all_users(void)
{
    std::ifstream file(USER_FILE);
    if (!file) return false;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string role_str, id, username, fname, lname, password;

        std::getline(ss, role_str, ',');
        std::getline(ss, id, ',');
        std::getline(ss, username, ',');
        std::getline(ss, fname, ',');
        std::getline(ss, lname, ',');
        std::getline(ss, password);

        Roles role = static_cast<Roles>(std::stoi(role_str));
        Str_pair fullname = {fname, lname};

        std::unique_ptr<User> user;
        switch (role)
        {
        case ADMIN:
            user = std::make_unique<Admin>(fullname, username, password, id, role);
            break;

        case MANAGER:
            user = std::make_unique<Manager>(fullname, username, password, id, role);
            break;

        case USER:
            user = std::make_unique<User>(fullname, username, password, id, role);
            break;

        default:
            continue;
        }

        attendance.push_back(UserAttendance
            {
                std::move(user),
                AttendanceRecord {}
            }
        );
    }

    return true;
}

bool Storage::save_attendance(void)
{
    std::ofstream file(ATTENDANCE_FILE);
    if (!file) return false;

    for (const auto &user_att : attendance)
    {
        const auto &uname = user_att.user->get_username();

        for (const auto &[date, entry] : user_att.logs)
        {
            file << uname << ',' << date << ','
                 << entry.login_time << ',' << entry.logout_time << ','
                 << entry.expected_login << ',' << entry.expected_logout << '\n';
        }
    }

    return true;
}

bool Storage::load_attendance(void)
{
    std::ifstream file(ATTENDANCE_FILE);
    if (!file) return false;

    std::string line;
    std::unordered_map<std::string, AttendanceRecord> logs_by_user;

    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string uname, date, in_time, out_time, expected_in, expected_out;

        if (!std::getline(ss, uname, ',')) continue;
        if (!std::getline(ss, date, ',')) continue;
        if (!std::getline(ss, in_time, ',')) continue;
        if (!std::getline(ss, out_time, ',')) continue;
        if (!std::getline(ss, expected_in, ',')) continue;
        if (!std::getline(ss, expected_out)) continue;

        logs_by_user[uname][date] = {in_time, out_time, expected_in, expected_out};
    }

    for (auto &user_att : attendance)
    {
        const std::string &uname = user_att.user->get_username();
        if (logs_by_user.count(uname))
        {
            user_att.logs = std::move(logs_by_user[uname]);
        }
    }

    return true;
}
