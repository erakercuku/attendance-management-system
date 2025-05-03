# global.R

# Install required packages if missing
required_packages <- c(
  "shiny", "shinydashboard", "dplyr", "ggplot2", "lubridate", "tidyr",
  "ggthemes", "viridis", "reshape2", "scales", "plotly", "forcats"
)

install_if_missing <- function(pkg) {
  if (!requireNamespace(pkg, quietly = TRUE)) install.packages(pkg)
}
invisible(lapply(required_packages, install_if_missing))

# Load all libraries
library(shiny)
library(shinydashboard)
library(dplyr)
library(ggplot2)
library(lubridate)
library(tidyr)
library(ggthemes)
library(viridis)
library(reshape2)
library(scales)
library(plotly)
library(forcats)


# ------------------------------
# DUMMY DATA INITIALIZATION
# ------------------------------

set.seed(42)
user_ids <- sprintf("user_%03d", 1:50)

# Users table
users_df <- data.frame(
  id = user_ids,
  registration_date = sample(seq(ymd("2024-01-01"), ymd("2024-04-30"), by = "day"), 50, replace = TRUE),
  account_status = sample(c("ACTIVE", "DEACTIVATED"), 50, replace = TRUE, prob = c(0.8, 0.2)),
  roles = sample(c("Admin", "User", "Moderator"), 50, replace = TRUE, prob = c(0.1, 0.7, 0.2)),
  stringsAsFactors = FALSE
)

# User metadata (university context)
user_data_df <- data.frame(
  id = user_ids,
  name = paste0("Name", 1:50),
  surname = paste0("Surname", 1:50),
  course = sample(c("CS", "Math", "Physics", NA), 50, replace = TRUE, prob = c(0.4, 0.3, 0.2, 0.1)),
  position = NA,
  stringsAsFactors = FALSE
)

# Module definitions
modules_df <- data.frame(
  module_id = sprintf("M%03d", 1:10),
  module_name = c("Linear Algebra", "Data Structures", "AI Ethics", "Operating Systems", "Databases",
                  "Statistics", "Machine Learning", "Computer Networks", "Quantum Computing", "Web Dev"),
  module_type = rep("Course", 10),
  stringsAsFactors = FALSE
)

# Schedule of sessions
start_base <- ymd_h("2024-05-06 09")
module_schedule_df <- data.frame()

for (mod in modules_df$module_id) {
  for (i in 1:2) {
    start_time <- start_base + days(sample(0:14, 1)) + hours(sample(0:2, 1))
    end_time <- start_time + hours(2)
    module_schedule_df <- rbind(module_schedule_df, data.frame(
      schedule_id = paste0(mod, "_S", i),
      module_id = mod,
      start_time = start_time,
      end_time = end_time
    ))
  }
}

# Enrollment table
user_module_df <- data.frame()
for (uid in user_ids) {
  enrolled_modules <- sample(modules_df$module_id, sample(1:4, 1))
  for (mod in enrolled_modules) {
    user_module_df <- rbind(user_module_df, data.frame(
      id = uid,
      module_id = mod,
      stringsAsFactors = FALSE
    ))
  }
}

# Attendance data (check-ins and check-outs)
user_actions_df <- data.frame
user_actions_df <- data.frame()

for (i in 1:nrow(user_module_df)) {
  user <- user_module_df$id[i]
  mod <- user_module_df$module_id[i]
  scheds <- module_schedule_df %>% filter(module_id == mod)
  
  for (j in 1:nrow(scheds)) {
    if (runif(1) < 0.75) {
      checkin <- scheds$start_time[j] + minutes(sample(0:10, 1))
      checkout <- scheds$end_time[j] - minutes(sample(0:10, 1))
      user_actions_df <- rbind(user_actions_df,
                               data.frame(id = user, action = "CHECKIN", time = checkin, module_id = mod),
                               data.frame(id = user, action = "CHECKOUT", time = checkout, module_id = mod))
    }
  }
}
user_actions_df$time <- as.POSIXct(user_actions_df$time, origin = "1970-01-01")

