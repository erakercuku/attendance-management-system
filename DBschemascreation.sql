CREATE TABLE Users (
    id CHAR(16) PRIMARY KEY,
    registration_date DATETIME NOT NULL,
    account_status ENUM('ACTIVE', 'DEACTIVATED') NOT NULL,
    roles ENUM('Admin', 'User', 'Moderator') NOT NULL
);

CREATE TABLE UserData (
    id CHAR(16) PRIMARY KEY,
    name VARCHAR(100) NOT NULL,
    surname VARCHAR(100) NOT NULL,
    course VARCHAR(100),      -- NULL if not university
    position VARCHAR(100),    -- NULL if not work
    FOREIGN KEY (id) REFERENCES Users(id)
);

CREATE TABLE Modules (
    module_id CHAR(8) PRIMARY KEY,
    module_name VARCHAR(255) NOT NULL,
    module_type ENUM('Course', 'Work') NOT NULL
);

CREATE TABLE ModuleSchedule (
    schedule_id VARCHAR(20) PRIMARY KEY,
    module_id CHAR(8) NOT NULL,
    start_time DATETIME NOT NULL,
    end_time DATETIME NOT NULL,
    FOREIGN KEY (module_id) REFERENCES Modules(module_id)
);

CREATE TABLE UserModules (
    id CHAR(16) NOT NULL,
    module_id CHAR(8) NOT NULL,
    PRIMARY KEY (id, module_id),
    FOREIGN KEY (id) REFERENCES Users(id),
    FOREIGN KEY (module_id) REFERENCES Modules(module_id)
);

CREATE TABLE UserActions (
    id CHAR(16) NOT NULL,
    module_id CHAR(8) NOT NULL,
    action ENUM('CHECKIN', 'CHECKOUT') NOT NULL,
    time DATETIME NOT NULL,
    PRIMARY KEY (id, time),
    FOREIGN KEY (id) REFERENCES Users(id),
    FOREIGN KEY (module_id) REFERENCES Modules(module_id)
);

ALTER TABLE UserActions
ADD COLUMN entry_type ENUM('automatic', 'manual') DEFAULT 'automatic',
ADD COLUMN modified_by CHAR(16),
ADD COLUMN modified_at DATETIME,
ADD FOREIGN KEY (modified_by) REFERENCES Users(id);
