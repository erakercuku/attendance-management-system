#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sqlite3.h>

std::string getCurrentTimestamp() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d-%H%M%S", ltm);
    return std::string(buf);
}

void backupUsers(sqlite3* db, const std::string& filePath) {
    std::ofstream file(filePath);
    file << "id,name,email\n";

    const char* sql = "SELECT id, name, email FROM users;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            file << sqlite3_column_int(stmt, 0) << ","
                 << sqlite3_column_text(stmt, 1) << ","
                 << sqlite3_column_text(stmt, 2) << "\n";
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to backup users.\n";
    }
    file.close();
}

void backupAttendance(sqlite3* db, const std::string& filePath) {
    std::ofstream file(filePath);
    file << "id,user_id,date,status\n";

    const char* sql = "SELECT id, user_id, date, status FROM attendance;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            file << sqlite3_column_int(stmt, 0) << ","
                 << sqlite3_column_int(stmt, 1) << ","
                 << sqlite3_column_text(stmt, 2) << ","
                 << sqlite3_column_text(stmt, 3) << "\n";
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Failed to backup attendance.\n";
    }
    file.close();
}

int main() {
    sqlite3* db;
    int rc = sqlite3_open("ams.db", &db);  // Use your SQLite DB name

    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << "\n";
        return 1;
    }

    std::string timestamp = getCurrentTimestamp();
    std::string usersFile = "users-" + timestamp + ".csv";
    std::string attendanceFile = "attendance-" + timestamp + ".csv";

    std::cout << "Backing up...\n";
    backupUsers(db, usersFile);
    backupAttendance(db, attendanceFile);
    std::cout << "Backup complete. Files saved: " << usersFile << " & " << attendanceFile << "\n";

    sqlite3_close(db);
    return 0;
}
