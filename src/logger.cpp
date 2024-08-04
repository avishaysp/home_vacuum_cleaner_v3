#include "logger.h"
#include <ctime>
#include <iomanip>
#include <format>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::log(LogLevel level, const std::string& message) {
    if (!logStream.is_open()) {
        openLogFile();
    }

    if (logStream.is_open()) {
        std::time_t now = std::time(nullptr);
        logStream << "[" << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << "] "
                  << getLogLevelString(level) << ": " << message << std::endl;
    } else {
        std::cerr << "Unable to write to log file: " << logFile << std::endl;
    }
    if (level == FATAL) {
        std::exit(EXIT_FAILURE);
    }
}

Logger::~Logger() {
    if (logStream.is_open()) {
        logStream.close();
    }
}

void Logger::openLogFile() {
    if (firstOpen) {
        logStream.open(logFile, std::ios_base::out | std::ios_base::trunc);
        firstOpen = false;
    } else {
        logStream.open(logFile, std::ios_base::out | std::ios_base::app);
    }
    if (!logStream.is_open()) {
        std::cerr << "Failed to open log file: " << logFile << std::endl;
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

