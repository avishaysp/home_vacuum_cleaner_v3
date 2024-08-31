// src/connon/logger.cpp
#include "logger.h"


Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLogFileName(const std::string& log_file_name) {
    std::thread::id thread_id = std::this_thread::get_id();
    std::lock_guard<std::mutex> lock(log_mutex);

    if (log_files.find(thread_id) != log_files.end()) {
        // Close the current log file if it is open
        if (log_streams.find(thread_id) != log_streams.end() && log_streams[thread_id].is_open()) {
            log_streams[thread_id].close();
        }
    }
    std::filesystem::path log_dir("log");
    std::filesystem::path full_path = log_dir / log_file_name;

    log_files[thread_id] = full_path.string();

    if (!std::filesystem::exists(log_dir)) {
        std::filesystem::create_directories(log_dir);
    }

    openLogFile(full_path.string());
}

void Logger::log(LogLevel level, const std::string& message, const std::string& file, int line) {
    std::thread::id thread_id = std::this_thread::get_id();
    std::optional<std::reference_wrapper<std::ofstream>> log_stream_opt;
    {
        std::lock_guard<std::mutex> lock(log_mutex);
        // Ensure the log file is open
        if (log_streams.find(thread_id) == log_streams.end() || !log_streams[thread_id].is_open()) {
            openLogFile(log_files[thread_id]);
        }
        if (log_streams[thread_id].is_open()) {
            log_stream_opt = log_streams[thread_id];
        }
    }

    if (!log_stream_opt) {
        return;
    }

    std::filesystem::path full_path = std::filesystem::absolute(file);
    std::filesystem::path current_path = std::filesystem::current_path();
    std::filesystem::path relative_path = std::filesystem::relative(full_path, current_path);

    std::ostringstream log_entry_stream;
    std::time_t now = std::time(nullptr);
    log_entry_stream  << "[" << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << "] "
                    << getLogLevelString(level) << ": "
                    << relative_path.string() << ":" << line << " | " << message;

    std::ofstream& log_stream = log_stream_opt.value();

    log_stream << log_entry_stream.str() << std::endl;
}

Logger::~Logger() {
    std::lock_guard<std::mutex> lock(log_mutex);
    for (auto& [id, stream] : log_streams) {
        if (stream.is_open()) {
            stream.close();
        }
    }
}

void Logger::openLogFile(const std::string& log_file_name) {
    std::thread::id thread_id = std::this_thread::get_id();

    if (log_streams.find(thread_id) != log_streams.end() && log_streams[thread_id].is_open()) {
        log_streams[thread_id].close();
    }

    log_streams[thread_id].open(log_file_name, std::ios_base::out | std::ios_base::trunc);

    if (!log_streams[thread_id].is_open()) {
        std::cerr << "Failed to open log file: " << log_file_name << " for thread: " << thread_id << std::endl;
    }
}

std::string Logger::getLogLevelString(LogLevel level) {
    switch (level) {
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}


Logger& logger = Logger::getInstance();

