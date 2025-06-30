#pragma once

#include <string>

enum class LogLevel {
    Debug,      // Debugging information
    Info,       // General runtime events, program flow information
    Warning,    // Something unexpected but not critical, potential issues
    Error,      // Errors that cause problems but not system failure
    Critical    // Severe error causing program termination or critical failure
};

void start_async_logger(const std::string& log_file_path);
void async_log(LogLevel log_level, const std::string& message);
void stop_async_logger();