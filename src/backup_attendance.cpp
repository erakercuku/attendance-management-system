#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <ctime>

using namespace std;
namespace fs = std::filesystem;

int main() {
    string source = "data/attendance.csv";

    // Create timestamp: attendance_YYYY-MM-DD.csv
    time_t now = time(nullptr);
    tm* local = localtime(&now);
    char dateStr[20];
    strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", local);
    string backupFile = "backups/attendance_" + string(dateStr) + ".csv";

    // Create backups/ folder if it doesn't exist
    fs::create_directories("backups");

    // Open log
    ofstream log("backups/backup_log.txt", ios::app);
    log << "[" << dateStr << "] ";

    try {
        fs::copy_file(source, backupFile, fs::copy_options::overwrite_existing);
        log << "Backup successful: " << backupFile << "\n";
        cout << "Backup completed: " << backupFile << endl;
    } catch (fs::filesystem_error& e) {
        log << "Backup failed: " << e.what() << "\n";
        cerr << "Backup failed: " << e.what() << endl;
    }

    log.close();
    return 0;
}
