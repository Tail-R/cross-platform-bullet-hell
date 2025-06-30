#include <iomanip>
#include <sstream> 
#include <fstream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <chrono>
#include <ctime>
#include "logger.hpp"

namespace {
    std::ofstream           log_file;
    std::queue<std::string> log_queue;
    std::mutex              mutex;
    std::condition_variable cond_var;
    std::thread             worker_thread;
    std::atomic<bool>       running{false};

    std::string get_timestamp() {
        using namespace std::chrono;

        // Acquire the current time
        auto now = system_clock::now();
        auto time_t_now = system_clock::to_time_t(now);
        auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

        std::tm buff;

#ifdef _WIN32
        localtime_s(&buff, &time_t_now);
#else
        localtime_r(&time_t_now, &buff);
#endif

        // Formatting
        std::ostringstream oss;

        oss << std::put_time(&buff, "[%Y-%m-%d %H:%M:%S");
        oss << '.' << std::setw(3) << std::setfill('0') << ms.count();
        oss << ']';

        return oss.str();
    }

    std::string log_level_to_string(LogLevel log_level) {
        switch (log_level)
        {
            case LogLevel::Debug:      return "[DEBUG]";
            case LogLevel::Info:       return "[INFO]";
            case LogLevel::Warning:    return "[WARNING]";
            case LogLevel::Error:      return "[ERROR]";
            case LogLevel::Critical:   return "[CRITICAL]";
            default:                   return "[UNKNOWN]";
        }
    }

    void writing_thread() {
        while (running || !log_queue.empty())
        {
            std::unique_lock<std::mutex> lock(mutex);

            // Wait until the log queue is not empty or logger is stoped
            cond_var.wait(lock, [] {
                return !log_queue.empty() || !running;
            });

            while (!log_queue.empty())
            {
                log_file << log_queue.front() << "\n";
                log_queue.pop();
            }

            log_file.flush();
        }
    }
}

void start_async_logger(const std::string& log_file_path) {
    // Already started
    if (running.exchange(true))
    {
        return;
    }

    log_file.open(log_file_path, std::ios::app);

    // Start worker thread
    worker_thread = std::thread(writing_thread);
}

void async_log(LogLevel log_level, const std::string& message) {
    if (!running)
    {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex);

    // Adding a timestamp
    std::string full_message = get_timestamp()
        + " "
        + log_level_to_string(log_level)
        + " "
        + message;
    
    log_queue.push(full_message);

    // Notify the thread to start writing log messages
    cond_var.notify_one();
}

void stop_async_logger() {
    // Already stoped
    if (!running.exchange(false))
    {
        return;
    }

    cond_var.notify_one();

    if (worker_thread.joinable())
    {
        worker_thread.join();
    }

    log_file.close();
}