// src/logger.cpp
#include "logger.h"

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLogFileName(const std::string& logFileName) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::thread::id threadId = std::this_thread::get_id();

    if (logFiles.find(threadId) != logFiles.end()) {
        // Close the current log file if it is open
        if (logStreams.find(threadId) != logStreams.end() && logStreams[threadId].is_open()) {
            // std::cout << "2" << std::endl;
            logStreams[threadId].close();
        }
    }
    logFiles[threadId] = logFileName;
    openLogFile(logFileName);
}

void Logger::log(LogLevel level, const std::string& message, const std::string& file, int line) {
    std::lock_guard<std::mutex> lock(logMutex);
    std::thread::id threadId = std::this_thread::get_id();

    // Ensure the log file is open
    if (logStreams.find(threadId) == logStreams.end() || !logStreams[threadId].is_open()) {
        openLogFile(logFiles[threadId]);
    }

    std::string filename = file;
    // Find the position of the last '/' or '\' in the file path
    size_t pos = file.find_last_of("/\\");
    if (pos != std::string::npos) {
        filename = file.substr(pos + 1);
    }

    if (logStreams[threadId].is_open()) {
        std::time_t now = std::time(nullptr);
        logStreams[threadId]    << "[" << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << "] "
                                << getLogLevelString(level) << ": "
                                << filename << ":" << line << " " << message << std::endl;
    }

    if (level == FATAL) {
        // TODO: switch to exception
        std::exit(EXIT_FAILURE);
    }
}

Logger::~Logger() {
    std::lock_guard<std::mutex> lock(logMutex);
    for (auto& [id, stream] : logStreams) {
        if (stream.is_open()) {
            stream.close();
        }
    }
}

void Logger::openLogFile(const std::string& logFileName) {
    std::thread::id threadId = std::this_thread::get_id();

    if (logStreams.find(threadId) != logStreams.end() && logStreams[threadId].is_open()) {
        logStreams[threadId].close();
    }

    logStreams[threadId].open(logFileName, std::ios_base::out | std::ios_base::trunc);

    if (!logStreams[threadId].is_open()) {
        // std::cerr << "Failed to open log file: " << logFileName << " for thread: " << threadId << std::endl;
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

