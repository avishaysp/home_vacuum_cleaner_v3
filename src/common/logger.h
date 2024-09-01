// src/connon/logger.h
#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <filesystem>

// uncomment this to enable logging:
// #define ENABLE_LOGGING

#define FILE_LOC __FILE__, __LINE__

#ifdef ENABLE_LOGGING
    #define LOG(level, message) logger.log(level, message, FILE_LOC)
#else
    #define LOG(level, message) do { } while (0)
#endif

enum LogLevel {
    INFO,
    WARNING,
    FATAL
};

class Logger {
public:
    static Logger& getInstance();
    void log(LogLevel level, const std::string& message, const std::string& file, int line);
    void setLogFileName(const std::string& log_file_name);

private:
    Logger() {}
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::unordered_map<std::thread::id, std::ofstream> log_streams;
    std::unordered_map<std::thread::id, std::string> log_files;

    std::mutex log_mutex;

    void openLogFile(const std::string& log_file_name);
    std::string getLogLevelString(LogLevel level);
};

class AlgorithmException : public std::exception {
public:
    explicit AlgorithmException(const std::string& message) : msg(message) {}
    virtual const char* what() const noexcept override { return msg.c_str(); }

private:
    std::string msg;
};

class HouseException : public std::exception {
public:
    explicit HouseException(const std::string& message) : msg(message) {}
    virtual const char* what() const noexcept override { return msg.c_str(); }

private:
    std::string msg;
};

extern Logger& logger;
