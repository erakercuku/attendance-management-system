## Introduction

This manual guides users and administrators on how to interact with the Attendance Management System. The system supports attendance tracking via login/logout, QR codes, and facial recognition with role-based access and analytics.

---

## User Roles

* **User** – Can log in/out, view personal attendance.
* **Manager** – Can view and export attendance reports for teams.
* **Admin** – Full access: modify roles, expected times, restore backups, and edit attendance.

---

## Getting Started

### 1. Registering

* Launch the CLI application.
* Select `Register`.
* Input:

  * Full Name
  * Unique Username (min. 5 ASCII characters)
  * Password (min. 8 characters, must contain upper/lowercase, digit, special character)
  * Role (assigned by Admin if needed)

### 2. Logging In

* Enter your username and password.
* Successful login records timestamp.
* Logout to record the end time.

### 3. Attendance View

* Access based on role:

  * Users: See personal logs.
  * Managers/Admins: See team-wide or full database.

### 4. QR Code Attendance (Manager/Admin Setup)

* A QR code is generated during user registration.
* Users scan QR to mark attendance.
* Requires camera and QR scanner module.

### 5. Facial Recognition (Admin Setup)

* Admin initiates face registration and validation process.
* On future logins, facial data is used for authentication.

### 6. Data Backups

* System performs automatic periodic backups.
* Manual restore possible via Admin panel.

### 7. Analytics

* R Shiny App dashboard displays:

  * Daily/weekly attendance
  * Punctuality statistics
  * User-specific reports

### 8. CSV Report Export

* Available to Managers and Admins.
* Export attendance logs to `.csv` from GUI or CLI.

