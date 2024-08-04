#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <format>

enum LogLevel {
    INFO,
    WARNING,
    FATAL
};

class Logger {
public:
    static Logger& getInstance();

    void log(LogLevel level, const std::string& message);

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

#endif // LOGGER_H
