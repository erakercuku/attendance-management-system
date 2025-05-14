#include "storage.hpp"
#include "authentication.hpp"
#include <sqlite3.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>

void Storage::save_user_to_db(const User &user)
{
    sqlite3* db;
    sqlite3_open("ams.db", &db);

    const char* sql = R"(
        INSERT INTO users (firstname, lastname, username, password, id, role)
        VALUES (?, ?, ?, ?, ?, ?);
    )";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

    // Bind values from the User object
    sqlite3_bind_text(stmt, 1, user.get_fullname().first.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, user.get_fullname().second.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, user.get_username().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, user.get_password().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, user.get_ID().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 6, static_cast<int>(user.get_role()));

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

bool Storage::load_user_from_db(const std::string &username, Str_pair &fullname, std::string &password, std::string &ID, Roles &role)
{
    sqlite3* db;
    sqlite3_stmt* stmt;
    bool found = false;

    if (sqlite3_open("ams.db", &db) != SQLITE_OK)
    {
        std::cerr << "Failed to open database.\n";
        return false;
    }

    const char* sql = "SELECT firstname, lastname, username, password, id, role FROM users WHERE username = ?;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "Failed to prepare statement.\n";
        sqlite3_close(db);
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        std::string fname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string lname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string uname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string pass = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string id    = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        std::string role_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        fullname = {fname, lname};
        password = pass;
        ID = id;
        role = static_cast<Roles>(std::stoi(role_str));
        found = true;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return found;
}

void Storage::load_users_from_db(void)
{
    sqlite3* db;
    sqlite3_stmt* stmt;

    if (sqlite3_open("ams.db", &db) != SQLITE_OK)
    {
        std::cerr << "Failed to open database.\n";
        return;
    }

    const char* sql = "SELECT username, password, role FROM users;";
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "Failed to prepare user SELECT.\n";
        sqlite3_close(db);
        return;
    }

    users.clear();

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        std::string fname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string lname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string uname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string pass = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        std::string role_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        Str_pair fullname;
        fullname.first = fname;
        fullname.second = lname;

        Roles role = (Roles) std::stoi(role_str);
    
        switch (role)
        {
        case ADMIN:
            users.push_back(std::make_unique<Manager>(fullname, uname, pass, id, ADMIN));
            break;

        case MANAGER:
            users.push_back(std::make_unique<Manager>(fullname, uname, pass, id, MANAGER));
            break;

        case USER:
            users.push_back(std::make_unique<Manager>(fullname, uname, pass, id, USER));
            break;

        default:
            break;
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void Storage::save_attendance_to_db(void)
{
    sqlite3* db;
    sqlite3_open("ams.db", &db);

    const char* create_sql = R"(
        CREATE TABLE IF NOT EXISTS attendance (
            username TEXT,
            date TEXT,
            login_time TEXT,
            logout_time TEXT,
            expected_login TEXT,
            expected_logout TEXT,
            PRIMARY KEY(username, date)
        );
    )";
    sqlite3_exec(db, create_sql, nullptr, nullptr, nullptr);

    const char* insert_sql = R"(
        INSERT INTO attendance (username, date, login_time, logout_time, expected_login, expected_logout)
        VALUES (?, ?, ?, ?, ?, ?)
        ON CONFLICT(username, date) DO UPDATE SET
            login_time = excluded.login_time,
            logout_time = excluded.logout_time,
            expected_login = excluded.expected_login,
            expected_logout = excluded.expected_logout;
    )";

    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db, insert_sql, -1, &stmt, nullptr);

    for (const auto &user_att : attendance)
    {
        const std::string &username = user_att.user->get_username();
        for (const auto &[date, daily] : user_att.logs)
        {
            sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, date.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, daily.login_time.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 4, daily.logout_time.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 5, daily.expected_login.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 6, daily.expected_logout.c_str(), -1, SQLITE_STATIC);

            sqlite3_step(stmt);
            sqlite3_reset(stmt);
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

void Storage::load_attendance_from_db(void)
{
    sqlite3* db;
    sqlite3_stmt* stmt;
    sqlite3_open("ams.db", &db);

    const char* sql = R"(
        SELECT username, date, login_time, logout_time, expected_login, expected_logout FROM attendance;
    )";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK)
    {
        std::cerr << "Failed to prepare attendance SELECT.\n";
        sqlite3_close(db);
        return;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        std::string uname = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        std::string date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string logout = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string expected_login = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        std::string expected_logout = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));

        auto it = std::find_if(attendance.begin(), attendance.end(),
            [&uname](const UserAttendance &ua)
            {
                return ua.user->get_username() == uname;
            });

        if (it == attendance.end())
        {
            continue;
        }

        DailyAttendance &log = it->logs[date];
        log.login_time = login;
        log.logout_time = logout;
        log.expected_login = expected_login;
        log.expected_logout = expected_logout;
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

bool Storage::save_user(const User &user)
{
    std::ifstream infile(USER_FILE);
    if (!infile) return false;

    std::vector<std::string> updated_lines;
    std::string line;
    bool user_found = false;

    while (std::getline(infile, line))
    {
        std::istringstream ss(line);
        std::string fname, lname, username, password, id, role_str;

        if (!std::getline(ss, fname, ',')) continue;
        if (!std::getline(ss, lname, ',')) continue;
        if (!std::getline(ss, username, ',')) continue;
        if (!std::getline(ss, password, ',')) continue;
        if (!std::getline(ss, id, ',')) continue;
        if (!std::getline(ss, role_str)) continue;

        if (id == user.get_ID())
        {
            std::ostringstream updated;
            updated << user.get_fullname().first << ','
                    << user.get_fullname().second << ','
                    << user.get_username() << ','
                    << user.get_password() << ','
                    << user.get_ID() << ','
                    << static_cast<int>(user.get_role());
            updated_lines.push_back(updated.str());
            user_found = true;
        }
        else
        {
            updated_lines.push_back(line);
        }
    }

    infile.close();

    if (!user_found)
    {
        std::ostringstream new_entry;
        new_entry << user.get_fullname().first << ','
                  << user.get_fullname().second << ','
                  << user.get_username() << ','
                  << user.get_password() << ','
                  << user.get_ID() << ','
                  << static_cast<int>(user.get_role());

        updated_lines.push_back(new_entry.str());
    }

    std::ofstream outfile(USER_FILE, std::ios::trunc);
    if (!outfile) return false;

    for (const auto &l : updated_lines)
    {
        outfile << l << '\n';
    }

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

bool Storage::load_all_users(void)
{
    std::ifstream file(USER_FILE);
    if (!file) return false;

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream ss(line);
        std::string role_str, id, username, fname, lname, password;

        if (!std::getline(ss, fname, ',')) continue;
        if (!std::getline(ss, lname, ',')) continue;
        if (!std::getline(ss, username, ',')) continue;
        if (!std::getline(ss, password, ',')) continue;
        if (!std::getline(ss, id, ',')) continue;
        if (!std::getline(ss, role_str)) continue;

        Roles role;
        try {
            role = (Roles) (std::stoi(role_str));
        }
        catch (const std::exception &e)
        {
            std::cerr << "Invalid role value: " << role_str << " in line: " << line << '\n';
            continue;
        }

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

        std::unique_ptr<User> attendance_user;

        switch (role)
        {
        case ADMIN:
            attendance_user = std::make_unique<Admin>(*static_cast<Admin*>(user.get()));
            break;
        case MANAGER:
            attendance_user = std::make_unique<Manager>(*static_cast<Manager*>(user.get()));
            break;
        case USER:
            attendance_user = std::make_unique<User>(*user);
            break;
        default:
            continue;
        }

        users.push_back(std::make_unique<User>(*user));
        attendance.push_back(UserAttendance
        {
            std::move(attendance_user),
            AttendanceRecord{}
        });
    }

    return true;
}

bool Storage::save_all_users(void)
{
    std::ofstream file(USER_FILE, std::ios::trunc);
    if (!file) return false;

    for (const auto &user_ptr : users)
    {
        if (!user_ptr) continue;

        file << user_ptr->get_fullname().first << ',' 
             << user_ptr->get_fullname().second << ','
             << user_ptr->get_username() << ','
             << user_ptr->get_password() << ','
             << user_ptr->get_ID() << ','
             << static_cast<int>(user_ptr->get_role()) << '\n';
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

void Storage::save_qr_id_to_csv(const std::string &id)
{
    std::ofstream file(HASHED_ID_FILE);
    if (!file) return;

    file << id << "\n";

    file.close();
}

User* Storage::search_user_by_username(const std::string &username)
{
    auto it = std::find_if(users.begin(), users.end(),
        [&username](const std::unique_ptr<User> &user)
        {
            return user->get_username() == username;
        });

    return (it != users.end()) ? it->get() : nullptr;
}
