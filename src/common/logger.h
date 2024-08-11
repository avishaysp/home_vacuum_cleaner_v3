// src/logger.h
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <format>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <thread>
#include <unordered_map>

#define FILE_LOC __FILE__, __LINE__

enum LogLevel {
    INFO,
    WARNING,
    FATAL
};

class Logger {
public:
    static Logger& getInstance();
    void log(LogLevel level, const std::string& message, const std::string& file, int line);
    void setLogFileName(const std::string& logFileName);

private:
    Logger() {}
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::unordered_map<std::thread::id, std::ofstream> logStreams;
    std::unordered_map<std::thread::id, std::string> logFiles;

    std::mutex logMutex;

    void openLogFile(const std::string& logFileName);
    std::string getLogLevelString(LogLevel level);
};

extern Logger& logger;
