-- Table: Users
CREATE TABLE Users (
    id CHAR(16) NOT NULL,
    registration_date DATETIME NOT NULL,
    account_status ENUM('ACTIVE', 'DEACTIVATED') NOT NULL,
    roles ENUM('Admin', 'User', 'Moderator') NOT NULL,
    PRIMARY KEY (id)
);

-- Table: user_data
CREATE TABLE user_data (
    id CHAR(16) NOT NULL,
    name VARCHAR(255) NOT NULL,
    surname VARCHAR(255) NOT NULL,
    course VARCHAR(255),
    position VARCHAR(255),
    PRIMARY KEY (id),
    CONSTRAINT fk_user_data_id FOREIGN KEY (id) REFERENCES Users(id)
);

-- Table: user_actions
CREATE TABLE user_actions (
    id CHAR(16) NOT NULL,
    action ENUM('CHECKIN', 'CHECKOUT') NOT NULL,
    time DATETIME NOT NULL,
    -- Using a composite primary key in case a user has multiple actions at different times.
    PRIMARY KEY (id, time),
    CONSTRAINT fk_user_actions_id FOREIGN KEY (id) REFERENCES Users(id)
);
