#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

using namespace std;
namespace fs = std::filesystem;

int main() {
    string backupName;
    cout << "\n🛠️ Restore Attendance\n";
    cout << "Enter the backup file name (e.g., attendance_2025-04-10.csv): ";
    getline(cin, backupName);

    string backupPath = "backups/" + backupName;
    string targetPath = "data/attendance.csv";

    if (!fs::exists(backupPath)) {
        cerr << "Backup file not found: " << backupPath << endl;
        return 1;
    }

    char confirm;
    cout << "This will overwrite the current attendance.csv. Continue? (y/n): ";
    cin >> confirm;

    if (confirm != 'y' && confirm != 'Y') {
        cout << "Restore cancelled.\n";
        return 0;
    }

    try {
        fs::copy_file(backupPath, targetPath, fs::copy_options::overwrite_existing);

        ofstream log("backups/restore_log.txt", ios::app);
        log << "[Restored] " << backupName << " -> attendance.csv\n";
        log.close();

        cout << "Restore complete: " << backupName << endl;
    } catch (fs::filesystem_error& e) {
        cerr << "Restore failed: " << e.what() << endl;
        return 1;
    }

    return 0;
}
