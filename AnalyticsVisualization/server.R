server <- function(input, output, session) {
  
  # Shared: Populate course dropdowns
  observe({
    course_choices <- unique(na.omit(user_data_df$course))
    
    updateSelectInput(session, "trend_course",
                      choices = c("All", course_choices),
                      selected = "All")
    
    updateSelectInput(session, "heatmap_course",
                      choices = c("All", course_choices),
                      selected = "All")
  })
  
  # ---- Tab 1: Attendance Trends ----
  output$attendance_trend_plot <- renderPlotly({
    course <- if (input$trend_course == "All") "none" else input$trend_course
    plot_attendance_trends(
      start_date = input$trend_start,
      end_date = input$trend_end,
      course_filter = course,
      user_actions_df = user_actions_df,
      user_data_df = user_data_df
    )
  })
  
  # ---- Tab 2: User Activity Heatmap ----
  output$activity_heatmap_plot <- renderPlotly({
    course <- if (input$heatmap_course == "All") "none" else input$heatmap_course
    plot_user_activity_heatmap(
      start_date = input$heatmap_start,
      end_date = input$heatmap_end,
      course_filter = course,
      user_actions_df = user_actions_df,
      user_data_df = user_data_df
    )
  })
  
  # ---- Tab 3: Status Distribution ----
  output$status_plot <- renderPlotly({
    plot_status_distribution(users_df)
  })
  
  # ---- Tab 4: Role Distribution ----
  output$role_plot <- renderPlotly({
    user_subset <- switch(input$role_status_filter,
                          "Active only" = users_df %>% filter(account_status == "ACTIVE"),
                          "Inactive only" = users_df %>% filter(account_status == "DEACTIVATED"),
                          users_df)
    plot_role_distribution(user_subset)
  })
  
  # ---- Tab 5: Inactive Users ----
  inactive_result <- reactive({
    plot_inactive_users(
      user_actions = user_actions_df,
      users = users_df,
      start_date = input$inactive_start,
      end_date = input$inactive_end
    )
  })
  
  output$inactive_plot <- renderPlot({
    # Calling the function for side effect: it prints the plot
    plot_inactive_users(
      user_actions = user_actions_df,
      users = users_df,
      start_date = input$inactive_start,
      end_date = input$inactive_end
    )
  })
  
  output$inactive_table <- renderDataTable({
    inactive_result()
  })
  

  # ---- Tab 6: Enrollment vs Attendance ----
  output$eva_plot <- renderPlotly({
    plot_enrolled_vs_attended(
      enrollments_df = user_module_df,
      attendance_df = user_actions_df,
      schedule_df = module_schedule_df,
      modules_df = modules_df,
      min_percent = input$eva_threshold / 100,
      start_date = input$eva_start,
      end_date = input$eva_end
    )
  })
  
  # ---- Tab 7: Avg Check-in Delay Over Time ----
  observe({
    course_choices <- unique(na.omit(user_data_df$course))
    updateSelectInput(session, "delay_course",
                      choices = c("All", course_choices),
                      selected = "All")
  })
  
  output$delay_trend_plot <- renderPlotly({
    course <- if (input$delay_course == "All") "none" else input$delay_course
    plot_average_checkin_delay(
      schedule_df = module_schedule_df,
      attendance_df = user_actions_df,
      user_data_df = user_data_df,
      group_by = input$delay_group,
      start_date = input$delay_start,
      end_date = input$delay_end,
      course_filter = course
    )
  })
  
  # ---- Tab 8: Avg Delay by Course or Module ----
  output$avgdelay_plot <- renderPlotly({
    plot_avg_delay(
      schedule_df = module_schedule_df,
      attendance_df = user_actions_df,
      user_data_df = user_data_df,
      modules_df = modules_df,
      start_date = input$avgdelay_start,
      end_date = input$avgdelay_end,
      by = input$avgdelay_by
    )
  })
  
  # ---- Tab 9: Top/Bottom N Students by Attendance ----
  output$attendrank_table <- DT::renderDT({
    get_attendance_rate_table(
      enrollments_df = user_module_df,
      attendance_df = user_actions_df,
      schedule_df = module_schedule_df,
      N = input$attendrank_n,
      direction = input$attendrank_dir
    )
  })
  
  # ---- Tab 10: Module Popularity ----
  output$modpop_plot <- renderPlotly({
    plot_top_modules_by_enrollment(
      enrollments_df = user_module_df,
      modules_df = modules_df,
      N = input$modpop_n
    )
  })
}
