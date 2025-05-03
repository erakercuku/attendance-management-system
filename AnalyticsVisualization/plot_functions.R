plot_attendance_trends <- function(start_date, end_date, course_filter = "none",
                                   user_actions_df, user_data_df) {
  
  start_date <- as.Date(start_date)
  end_date <- as.Date(end_date)
  
  merged_df <- user_actions_df %>%
    left_join(user_data_df, by = "id") %>%
    mutate(date = as.Date(time)) %>%
    filter(date >= start_date & date <= end_date)
  
  if (tolower(course_filter) != "none") {
    merged_df <- merged_df %>% filter(course == course_filter)
  }
  
  filtered_df <- merged_df %>%
    group_by(date, action) %>%
    summarise(count = n(), .groups = "drop") %>%
    tidyr::pivot_wider(names_from = action, values_from = count, values_fill = 0)
  
  # Make sure CHECKIN / CHECKOUT exist even if 0
  if (!"CHECKIN" %in% colnames(filtered_df)) filtered_df$CHECKIN <- 0
  if (!"CHECKOUT" %in% colnames(filtered_df)) filtered_df$CHECKOUT <- 0
  
  trend_long <- pivot_longer(filtered_df,
                             cols = c("CHECKIN", "CHECKOUT"),
                             names_to = "action", values_to = "count")
  
  (ggplot(trend_long, aes(x = date, y = count, color = action)) +
    geom_line(size = 1.2) +
    geom_point() +
    labs(
      title = paste("Attendance Trends:",
                    ifelse(course_filter == "none", "All Courses", paste("Course:", course_filter)),
                    "\n", start_date, "to", end_date),
      x = "Date",
      y = "Number of Actions",
      color = "Action Type"
    ) +
    theme_minimal() +
    scale_color_manual(values = c("CHECKIN" = "steelblue", "CHECKOUT" = "darkorange")) +
    theme(axis.text.x = element_text(angle = 45, hjust = 1)))%>%ggplotly()
}


plot_user_activity_heatmap <- function(start_date, end_date, course_filter = "none",
                                       user_actions_df, user_data_df) {
  
  start_date <- as.Date(start_date)
  end_date <- as.Date(end_date)
  
  merged_df <- user_actions_df %>%
    left_join(user_data_df, by = "id") %>%
    mutate(date = as.Date(time)) %>%
    filter(date >= start_date & date <= end_date)
  
  if (tolower(course_filter) != "none") {
    merged_df <- merged_df %>% filter(course == course_filter)
  }
  
  activity_df <- merged_df %>%
    group_by(date) %>%
    summarise(activity_count = n(), .groups = "drop") %>%
    mutate(
      weekday = wday(date, label = TRUE, week_start = 1),
      week = isoweek(date)
    )
  
  (ggplot(activity_df, aes(x = factor(week), y = fct_rev(weekday), fill = activity_count)) +
    geom_tile(color = "white") +
    scale_fill_viridis(name = "Activity", option = "C", direction = -1) +
    labs(
      title = paste("User Activity Heatmap",
                    ifelse(course_filter == "none", "(All Courses)", paste("(", course_filter, ")")),
                    "\n", start_date, "to", end_date),
      x = "ISO Week",
      y = "Weekday"
    ) +
    theme_minimal() +
    theme(axis.text.x = element_text(angle = 45, hjust = 1)))%>%ggplotly()
}


plot_status_distribution <- function(users_df) {
  
  # Summarize status
  status_summary <- users_df %>%
    count(account_status) %>%
    mutate(percentage = round(100 * n / sum(n), 1))
  
  # Create pie chart using plotly
  plot_ly(status_summary,
          labels = ~account_status,
          values = ~n,
          type = 'pie',
          textinfo = 'label+percent',
          insidetextorientation = 'radial',
          marker = list(colors = c("steelblue", "tomato"))) %>%
    layout(title = "User Status Distribution")
}

plot_role_distribution <- function(data) {
  
  # Determine label from filtering (optional for clarity)
  status_label <- case_when(
    all(data$account_status == "ACTIVE") ~ " (Active Users)",
    all(data$account_status == "DEACTIVATED") ~ " (Inactive Users)",
    TRUE ~ " (All Users)"
  )
  
  # Summarize role counts
  role_summary <- data %>%
    count(roles) %>%
    mutate(percentage = round(100 * n / sum(n), 1))
  
  # Plot
  (ggplot(role_summary, aes(x = reorder(roles, -n), y = n, fill = roles)) +
    geom_bar(stat = "identity", width = 0.6) +
    geom_text(aes(label = paste0(percentage, "%")), vjust = -0.3) +
    labs(
      title = paste0("User Role Distribution", status_label),
      x = "Role",
      y = "Number of Users",
      fill = "Role"
    ) +
    theme_minimal() +
    scale_fill_manual(values = c("Admin" = "darkgreen", "User" = "steelblue", "Moderator" = "orange")))%>%ggplotly()
}


plot_inactive_users <- function(user_actions, users, start_date, end_date) {
  
  # Ensure date types
  start_date <- as.Date(start_date)
  end_date <- as.Date(end_date)
  
  # Get last check-in per user
  user_checkins <- user_actions %>%
    filter(action == "CHECKIN") %>%
    mutate(date = as.Date(time)) %>%
    group_by(id) %>%
    summarise(last_checkin = max(date), .groups = "drop")
  
  # Join with users table
  user_status <- users %>%
    select(id) %>%
    left_join(user_checkins, by = "id") %>%
    mutate(inactive = is.na(last_checkin) | last_checkin < start_date) %>%
    filter(inactive == TRUE)
  
  # Count of inactive users
  count_inactive <- nrow(user_status)
  
  # Plot: single bar with label
  p <- ggplot(data.frame(count = count_inactive), aes(x = "", y = count)) +
    geom_bar(stat = "identity", fill = "firebrick") +
    geom_text(aes(label = count), vjust = -0.5, size = 6) +
    labs(title = paste("Number of Inactive Users (No Check-in from", start_date, "to", end_date, ")"),
         x = NULL, y = "Count") +
    theme_minimal() +
    theme(axis.text.x = element_blank(),
          axis.ticks.x = element_blank())
  
  print(p)
  
  # Return table of inactive users and last check-in
  return(user_status %>% select(id, last_checkin))
}


plot_enrolled_vs_attended <- function(
    enrollments_df,
    attendance_df,
    schedule_df,
    modules_df,
    min_percent = 0.5,
    start_date = NULL,
    end_date = NULL
) {
  
  # Convert and apply date filters if provided
  if (!is.null(start_date)) start_date <- as.Date(start_date)
  if (!is.null(end_date)) end_date <- as.Date(end_date)
  
  if (!is.null(start_date) & !is.null(end_date)) {
    schedule_df <- schedule_df %>%
      filter(as.Date(start_time) >= start_date & as.Date(end_time) <= end_date)
    
    attendance_df <- attendance_df %>%
      filter(as.Date(time) >= start_date & as.Date(time) <= end_date)
  }
  
  # 1. Enrolled count per module
  enrolled <- enrollments_df %>%
    group_by(module_id) %>%
    summarise(enrolled = n_distinct(id), .groups = "drop")
  
  # 2. Total sessions per module
  session_counts <- schedule_df %>%
    group_by(module_id) %>%
    summarise(total_sessions = n(), .groups = "drop")
  
  # 3. CHECKIN counts per user per module
  attendance_checkins <- attendance_df %>%
    filter(action == "CHECKIN") %>%
    group_by(module_id, id) %>%
    summarise(attended_sessions = n(), .groups = "drop")
  
  # 4. Join and calculate active status
  attendance_detail <- attendance_checkins %>%
    left_join(session_counts, by = "module_id") %>%
    mutate(attendance_pct = attended_sessions / total_sessions) %>%
    mutate(active = attendance_pct >= min_percent)
  
  # 5. Summary per module
  attendance_summary <- attendance_detail %>%
    group_by(module_id) %>%
    summarise(
      attended = n(),
      active_attended = sum(active),
      .groups = "drop"
    )
  
  # 6. Final merge
  summary_df <- enrolled %>%
    left_join(attendance_summary, by = "module_id") %>%
    replace_na(list(attended = 0, active_attended = 0)) %>%
    left_join(modules_df, by = "module_id") %>%
    pivot_longer(cols = c("enrolled", "attended", "active_attended"),
                 names_to = "type", values_to = "count")
  
  # 7. Plot
  (ggplot(summary_df, aes(x = reorder(module_name, -count), y = count, fill = type)) +
    geom_bar(stat = "identity", position = "dodge") +
    labs(
      title = paste(
        "Enrolled vs Attended vs Active",
        if (!is.null(start_date)) paste0(" (", start_date, " to ", end_date, ")") else "",
        "\n(Active = ≥", min_percent * 100, "% Sessions)"
      ),
      x = "Module",
      y = "Number of Users",
      fill = "User Group"
    ) +
    theme_minimal() +
    theme(axis.text.x = element_text(angle = 45, hjust = 1)) +
    scale_fill_manual(values = c(
      "enrolled" = "steelblue",
      "attended" = "orange",
      "active_attended" = "darkgreen"
    )))%>%ggplotly()
}



plot_average_checkin_delay <- function(
    schedule_df,         # module_schedule_df
    attendance_df,       # user_actions_df
    user_data_df,        # to filter by course
    group_by = "daily",  # "daily" or "weekly"
    start_date = NULL,
    end_date = NULL,
    course_filter = "none"
) {
  
  # Ensure proper dates
  if (!is.null(start_date)) start_date <- as.Date(start_date)
  if (!is.null(end_date)) end_date <- as.Date(end_date)
  
  # Filter schedule
  sched_filtered <- schedule_df
  if (!is.null(start_date)) sched_filtered <- sched_filtered %>% filter(as.Date(start_time) >= start_date)
  if (!is.null(end_date)) sched_filtered <- sched_filtered %>% filter(as.Date(end_time) <= end_date)
  
  # Filter attendance (only check-ins)
  checkins <- attendance_df %>%
    filter(action == "CHECKIN") %>%
    left_join(user_data_df, by = "id")
  
  if (tolower(course_filter) != "none") {
    checkins <- checkins %>% filter(course == course_filter)
  }
  
  # Join check-ins to matching session and compute delay
  join_df <- checkins %>%
    inner_join(sched_filtered, by = "module_id") %>%
    filter(as.Date(time) == as.Date(start_time)) %>%
    mutate(delay_minutes = as.numeric(difftime(time, start_time, units = "mins")))
  
  # Grouping
  join_df <- join_df %>%
    mutate(group_date = case_when(
      group_by == "weekly" ~ paste0(year(start_time), "-W", isoweek(start_time)),
      TRUE ~ as.character(as.Date(start_time))
    ))
  
  # Summarise average delay
  avg_delay_df <- join_df %>%
    group_by(group_date) %>%
    summarise(avg_delay = mean(delay_minutes), .groups = "drop")
  
  # Plot
  (ggplot(avg_delay_df, aes(x = group_date, y = avg_delay)) +
    geom_line(group = 1, color = "steelblue", size = 1.2) +
    geom_point(color = "darkorange", size = 2) +
    geom_hline(yintercept = 0, linetype = "dashed", color = "gray50") +
    labs(
      title = paste("Average Check-in Delay",
                    if (course_filter != "none") paste("for Course:", course_filter),
                    if (!is.null(start_date) & !is.null(end_date)) paste0("\n(", start_date, " to ", end_date, ")") else ""),
      x = ifelse(group_by == "weekly", "Week", "Date"),
      y = "Avg Delay (min; negative = early)"
    ) +
    theme_minimal() +
    theme(axis.text.x = element_text(angle = 45, hjust = 1)))%>%ggplotly()
}

plot_avg_delay <- function(
    schedule_df,         # module_schedule_df
    attendance_df,       # user_actions_df
    user_data_df,        # user_data_df
    modules_df,          # modules_df (for module names)
    start_date = NULL,
    end_date = NULL,
    by = "course"         # "course" or "module"
) {
  
  # Convert dates if provided
  if (!is.null(start_date)) start_date <- as.Date(start_date)
  if (!is.null(end_date)) end_date <- as.Date(end_date)
  
  # Filter schedule
  sched_filtered <- schedule_df
  if (!is.null(start_date)) sched_filtered <- sched_filtered %>% filter(as.Date(start_time) >= start_date)
  if (!is.null(end_date)) sched_filtered <- sched_filtered %>% filter(as.Date(end_time) <= end_date)
  
  # Only CHECKINs, merge with user and course
  checkins <- attendance_df %>%
    filter(action == "CHECKIN") %>%
    left_join(user_data_df, by = "id") %>%
    left_join(modules_df, by = "module_id")  # for module_name
  
  # Join check-ins to sessions
  join_df <- checkins %>%
    inner_join(sched_filtered, by = "module_id") %>%
    filter(as.Date(time) == as.Date(start_time)) %>%
    mutate(delay_minutes = as.numeric(difftime(time, start_time, units = "mins")))
  
  # Group and summarize
  if (by == "course") {
    plot_df <- join_df %>%
      filter(!is.na(course)) %>%
      group_by(course) %>%
      summarise(avg_delay = mean(delay_minutes), .groups = "drop") %>%
      mutate(label = course)
    x_title <- "Course"
  } else if (by == "module") {
    plot_df <- join_df %>%
      group_by(module_name) %>%
      summarise(avg_delay = mean(delay_minutes), .groups = "drop") %>%
      mutate(label = module_name)
    x_title <- "Module"
  } else {
    stop("`by` must be 'course' or 'module'")
  }
  
  # Plot
  (ggplot(plot_df, aes(x = reorder(label, avg_delay), y = avg_delay, fill = avg_delay)) +
    geom_bar(stat = "identity", width = 0.6) +
    geom_text(aes(label = round(avg_delay, 1)), vjust = -0.5) +
    scale_fill_gradient2(low = "green", mid = "gray90", high = "firebrick", midpoint = 0) +
    labs(
      title = paste("Average Check-in Delay by", tools::toTitleCase(by),
                    if (!is.null(start_date) & !is.null(end_date))
                      paste0(" (", start_date, " to ", end_date, ")") else ""),
      x = x_title,
      y = "Avg Delay (min; negative = early)"
    ) +
    theme_minimal() +
    theme(axis.text.x = element_text(angle = 45, hjust = 1), legend.position = "none"))%>%ggplotly()
}



get_attendance_rate_table <- function(enrollments_df, attendance_df, schedule_df, N = 10, direction = "top") {
  
  # Total expected sessions per student
  expected_sessions <- enrollments_df %>%
    left_join(schedule_df, by = "module_id") %>%
    group_by(id) %>%
    summarise(expected = n(), .groups = "drop")
  
  # Actual attended sessions
  attended_sessions <- attendance_df %>%
    filter(action == "CHECKIN") %>%
    group_by(id) %>%
    summarise(attended = n(), .groups = "drop")
  
  # Combine and calculate attendance %
  attendance_rate <- expected_sessions %>%
    left_join(attended_sessions, by = "id") %>%
    mutate(
      attended = replace_na(attended, 0),
      attendance_rate = round(100 * attended / expected, 1)
    )
  
  # Sort and select top/bottom N
  result <- attendance_rate %>%
    arrange(if (direction == "top") desc(attendance_rate) else attendance_rate) %>%
    head(N)
  
  return(result)
}



plot_top_modules_by_enrollment <- function(enrollments_df, modules_df, N = 10) {
  
  # Count enrollments
  top_modules <- enrollments_df %>%
    count(module_id, name = "num_enrolled") %>%
    left_join(modules_df, by = "module_id") %>%
    arrange(desc(num_enrolled)) %>%
    head(N)
  
  # Plot
  (ggplot(top_modules, aes(x = reorder(module_name, num_enrolled), y = num_enrolled)) +
    geom_bar(stat = "identity", fill = "steelblue") +
    geom_text(aes(label = num_enrolled), vjust = -0.3) +
    labs(
      title = paste("Top", N, "Modules by Enrollment"),
      x = "Module",
      y = "Number of Enrolled Users"
    ) +
    theme_minimal() +
    theme(axis.text.x = element_text(angle = 45, hjust = 1)))%>%ggplotly()
}
