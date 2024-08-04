// src/logger.h
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <format>

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

private:
    Logger() : logFile("vacuum_cleaner.log"), firstOpen(true) {}
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream logStream;
    std::string logFile;
    bool firstOpen;

    void openLogFile();
    std::string getLogLevelString(LogLevel level);
};

extern Logger& logger;
