# Attendance Management System

## Overview
The Attendance Management System is developed and designed in such a way that it helps to automate the process of tracking the attendance. It is a comprehensive platform designed for educational or organizational environments to manage user registrations, attendance tracking, and analytics through a multi-role interface. The system uses CLI, QR codes, and facial recognition for marking attendance, and provides administrators with tools for managing users and attendance records. The system includes permission based on the roles of the user to access the controls ensuring secure and efficient attendance logging, storage, and reporting. It integrates multiple technologies including **C++**, **Python**, **MySQL**, **R (Shiny App)**, and more.

## Features

### Authentication & User Management
- User registration with input validation and credential creation
- Secure login using hashed passwords and unique IDs
- Role-based access control (Admin, Manager, Employee)
- Admins can assign and modify user roles
- Input validation for all credentials and roles

### Attendance Management
- Automated attendance marking with login/logout timestamps
- Admins can set and modify expected login/logout times
- View attendance based on user roles
- Local backups for attendance recovery

### Analytics & Reporting
- R Shiny App-based analytics with graphs
- Role-based access to reports and attendance trends
- Exportable attendance data in CSV format

### QR Code & Facial Recognition
- Unique QR code generation for each user
- QR-based login integration
- Facial recognition (Python module) for future enhancement

### Data Persistence
- MySQL database for core data storage
- Periodic local backups for data recovery
- Modular file and directory structure

## Technologies Used

- **C++** – Core logic and CLI system
- **Python** – QR code generation and facial recognition
- **MySQL** – Relational database for persistent data storage
- **Shiny R App** – Analytics dashboard and report visualization
- **Shell scripting / CRON** – Periodic backup automation
