run_app <- function() {
  shinyApp(
    ui = source("ui.R")$value,
    server = source("server.R")$value
  )
}

run_app()
