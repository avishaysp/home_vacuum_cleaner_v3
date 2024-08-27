// src/connon/logger.cpp
#include "logger.h"


Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

void Logger::setLogFileName(const std::string& logFileName) {
    std::thread::id threadId = std::this_thread::get_id();
    std::lock_guard<std::mutex> lock(logMutex);

    if (logFiles.find(threadId) != logFiles.end()) {
        // Close the current log file if it is open
        if (logStreams.find(threadId) != logStreams.end() && logStreams[threadId].is_open()) {
            logStreams[threadId].close();
        }
    }
    std::filesystem::path logDir("log");
    std::filesystem::path fullPath = logDir / logFileName;

    logFiles[threadId] = fullPath.string();

    if (!std::filesystem::exists(logDir)) {
        std::filesystem::create_directories(logDir);
    }

    openLogFile(fullPath.string());
}

void Logger::log(LogLevel level, const std::string& message, const std::string& file, int line) {
    std::thread::id threadId = std::this_thread::get_id();
    {
        std::lock_guard<std::mutex> lock(logMutex);
        // Ensure the log file is open
        if (logStreams.find(threadId) == logStreams.end() || !logStreams[threadId].is_open()) {
            openLogFile(logFiles[threadId]);
        }
    }

    std::filesystem::path fullPath = std::filesystem::absolute(file);
    std::filesystem::path currentPath = std::filesystem::current_path();
    std::filesystem::path relativePath = std::filesystem::relative(fullPath, currentPath);

    std::ostringstream logEntryStream;
    std::time_t now = std::time(nullptr);
    logEntryStream  << "[" << std::put_time(std::localtime(&now), "%Y-%m-%d %H:%M:%S") << "] "
                    << getLogLevelString(level) << ": "
                    << relativePath.string() << ":" << line << " | " << message;

    // no locking, since each thread has its own log
    if (logStreams[threadId].is_open()) {
        logStreams[threadId] << logEntryStream.str() << std::endl;
    }

    if (level == FATAL) {
        throwRelevantException(logEntryStream.str(), relativePath.string());
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
        std::cerr << "Failed to open log file: " << logFileName << " for thread: " << threadId << std::endl;
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


void Logger::throwRelevantException(const std::string& message, const std::string& file) {
    if (file.find("algorithm") != std::string::npos) {
        throw AlgorithmException(message);
    }
    throw HouseException(message);
}


Logger& logger = Logger::getInstance();

