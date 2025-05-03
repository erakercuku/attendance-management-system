ui <- navbarPage("Attendance Analytics Dashboard",
                 
                 # ---- Tab 1 ----
                 tabPanel("1. Attendance Trends",
                          sidebarLayout(
                            sidebarPanel(
                              dateInput("trend_start", "Start Date", value = Sys.Date() - 30),
                              dateInput("trend_end", "End Date", value = Sys.Date()),
                              selectInput("trend_course", "Filter by Course", choices = NULL)  # dynamic update below
                            ),
                            mainPanel(
                              plotlyOutput("attendance_trend_plot")
                            )
                          )
                 ),
                 
                 # ---- Tab 2 ----
                 tabPanel("2. User Activity Heatmap",
                          sidebarLayout(
                            sidebarPanel(
                              dateInput("heatmap_start", "Start Date", value = Sys.Date() - 30),
                              dateInput("heatmap_end", "End Date", value = Sys.Date()),
                              selectInput("heatmap_course", "Filter by Course", choices = NULL)
                            ),
                            mainPanel(
                              plotlyOutput("activity_heatmap_plot")
                            )
                          )
                 ),
                 
                 # ---- Tab 3 to 10 ----
                 tabPanel("3. Status Distribution",
                          fluidPage(
                            h4("User Status Breakdown"),
                            plotlyOutput("status_plot")
                          )
                 ),
                 tabPanel("4. Role Distribution",
                          sidebarLayout(
                            sidebarPanel(
                              selectInput("role_status_filter", "User Status:",
                                          choices = c("All", "Active only", "Inactive only"),
                                          selected = "All")
                            ),
                            mainPanel(
                              plotlyOutput("role_plot")
                            )
                          )
                 ),
                 tabPanel("5. Inactive Users",
                          sidebarLayout(
                            sidebarPanel(
                              dateInput("inactive_start", "Start Date", value = Sys.Date() - 30),
                              dateInput("inactive_end", "End Date", value = Sys.Date())
                            ),
                            mainPanel(
                              plotOutput("inactive_plot"),
                              br(),
                              h5("Inactive Users and Last Check-in Date"),
                              dataTableOutput("inactive_table")
                            )
                          )
                 ),
                 tabPanel("6. Enrollment vs Attendance",
                          sidebarLayout(
                            sidebarPanel(
                              dateInput("eva_start", "Start Date", value = Sys.Date() - 30),
                              dateInput("eva_end", "End Date", value = Sys.Date()),
                              sliderInput("eva_threshold", "Active Attendance Threshold (%)",
                                          min = 0, max = 100, value = 50, step = 5)
                            ),
                            mainPanel(
                              plotlyOutput("eva_plot")
                            )
                          )
                 ),
                 tabPanel("7. Avg Check-in Delay Over Time",
                          sidebarLayout(
                            sidebarPanel(
                              dateInput("delay_start", "Start Date", value = Sys.Date() - 30),
                              dateInput("delay_end", "End Date", value = Sys.Date()),
                              selectInput("delay_course", "Filter by Course", choices = NULL),
                              radioButtons("delay_group", "Group by:", choices = c("Daily" = "daily", "Weekly" = "weekly"), selected = "daily")
                            ),
                            mainPanel(
                              plotlyOutput("delay_trend_plot")
                            )
                          )
                 ),
                 tabPanel("8. Avg Delay by Course/Module",
                          sidebarLayout(
                            sidebarPanel(
                              dateInput("avgdelay_start", "Start Date", value = Sys.Date() - 30),
                              dateInput("avgdelay_end", "End Date", value = Sys.Date()),
                              radioButtons("avgdelay_by", "Group By:", choices = c("Course" = "course", "Module" = "module"), selected = "course")
                            ),
                            mainPanel(
                              plotlyOutput("avgdelay_plot")
                            )
                          )
                 ),
                 tabPanel("9. Attendance Rate Rankings",
                          sidebarLayout(
                            sidebarPanel(
                              numericInput("attendrank_n", "Number of Students to Show", value = 10, min = 1, max = 50),
                              radioButtons("attendrank_dir", "Show:", choices = c("Top" = "top", "Bottom" = "bottom"), selected = "top")
                            ),
                            mainPanel(
                              dataTableOutput("attendrank_table")
                            )
                          )
                 ),
                 tabPanel("10. Module Popularity",
                          sidebarLayout(
                            sidebarPanel(
                              numericInput("modpop_n", "Number of Top Modules", value = 10, min = 1, max = length(unique(user_module_df$module_id)))
                            ),
                            mainPanel(
                              plotlyOutput("modpop_plot")
                            )
                          )
                 )
                 
)
