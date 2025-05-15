
## Attendance Management System – Documentation

---

## 1. Introduction

The **Attendance Management System** is a secure, role-based application designed to record and monitor user attendance using **QR codes** and **facial recognition**. It provides administrators and employees with a way to **track login/logout times**, **generate reports**, and **manage roles**. Designed for use in workplaces or educational institutions, it solves the problem of **manual attendance errors**, **unauthorized logins**, and **inconsistent data tracking**.

---

## 2. System Overview

The system follows a modular structure with distinct components:

- **User Authentication (CLI)**: Register, login, and manage roles.
- **QR Code System**: Unique QR generation per user.
- **Facial Recognition Module**: Optional biometric attendance validation.
- **Attendance Logger**: Auto time-stamped attendance logging.
- **Database (MySQL)**: Central storage for users, roles, and attendance.
- **Backup System**: Regular local and remote backups.
- **R Shiny GUI**: Graphical interface for attendance analytics and reports.

---

## 3. Features

| Feature                  | Description |
|--------------------------|-------------|
| User Registration     | CLI interface to register new users with roles |
| Login Validation      | Credential validation with encryption |
| Role Management     | Role-based privileges (admin, staff, guest) |
| Attendance Logging     | Automatic login/logout with timestamps |
| QR Code Generation    | Each user has a unique QR code |
| Facial Recognition  | Optional login verification via webcam |
| Local Backups         | User data is periodically saved locally |
| R Shiny Analytics     | View attendance summaries and charts |
| Report Export         | Admins can export attendance as CSV |
| Manual Edits          | Admins can edit login/logout records |
| Data Recovery         | Restores from latest valid backup in case of crash |

---

## 4. Technologies Used

* **C++** – Main application logic, CLI
* **Python** – QR code generation, face recognition
* **R + Shiny** – Attendance dashboard and analytics
* **MySQL** – Persistent data storage
* **ClickUp + GitHub** – Task and version control
* **Confluence** – Project coordination
* **OpenCV / Pillow** – Face recognition and image processing
* **qrcode / hashlib** – Python libraries for QR and hashing

---

## 5. Installation Instructions

### Requirements

* `g++`, `Python 3`, `MySQL`, `R`, `Shiny`, `OpenCV`, `qrcode`, `pip`

### Steps

1. Clone the repository:

   ```bash
   git clone https://github.com/your-username/attendance-system.git
   cd attendance-system
   ```

2. Install Python dependencies:

   ```bash
   pip install qrcode opencv-python pillow mysql-connector-python
   ```

3. Set up the MySQL database:

   * Run the schema file located in `db/schema.sql` in your MySQL server.
   * Update DB credentials in the Python/C++ files as needed.

4. Run the C++ application:

   ```bash
   make project
   ./project
   ```

5. Run the Shiny GUI (if installed):

   * Open `gui/shiny_app.R` using RStudio or:

     ```R
     shiny::runApp("gui/")
     ```

---

## 6. How to Use

### As a User

* Register via the CLI
* Use your QR code or face scan to log in
* View your attendance via Shiny (if available)

### As an Admin

* Register other users
* Edit expected login/logout times
* Export attendance reports
* Recover data using the backup tool
* Manage roles and permissions

---

## 7. Developer Guide

### Key Notes

* Database credentials should be configured in a separate `.env` or config file
* All modules are loosely coupled for easier refactoring
* CLI supports menu navigation and input validation
* Shiny pulls data using direct SQL queries

---

## 8. Testing

* Input validation for registration (edge cases, invalid inputs)
* Login attempts with wrong credentials
* QR and face match validation
* Admin permission enforcement
* Crash test recovery using backup
* Attendance report generation with null values
* Exported CSV matches DB data
* UI tested for usability in Shiny

---

## 9. Known Issues / Limitations

* GUI is not fully connected to backend C++ code
* Facial recognition has occasional false negatives
* No mobile interface (future work)
* Limited unit test automation
* Cross-platform support needs testing (Windows/Linux)

---

## 10. Sprint History

### Sprint 1: Foundation

* User registration, login, basic DB
* Role assignment, file structure

### Sprint 2: Validation & QR

* Credential hashing, QR code generation
* Role-based attendance view

### Sprint 3: Analytics & Refactor

* Analytics via Shiny, refactored core logic
* Finalized QR, backups, time editing

### Sprint 4: GUI Integration & Finalization

* GUI connected, CSV export, documentation, presentation

---

## 11. Contributors

| Name         | Role                                         |
| ------------ | -------------------------------------------- |
| **Haziel**   | Core logic, registration, encryption, backup |
| **Stuti**    | Roles, GUI, editing times, reporting         |
| **Valentyn** | QR code, face recognition, analytics         |
| **Era**      | CLI, local backups, documentation            |


