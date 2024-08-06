// io_handling.h

#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include "house.h"
#include "Path.h"
#include "logger.h"
#include "status.h"


class FileReader {
    std::string file_path;

    std::vector<std::string> split(const std::string &str, const char delimiter) const;
    size_t strToSize_t(const std::string &str) const;
    std::string trim(const std::string &str) const;
    Location parseLocation(const std::string& str) const;
    size_t readArgument(const std::string& str) const;
    void ParseHouse(std::ifstream &file, std::shared_ptr<House> house) const;

public:
    FileReader(const std::string& file_path);

    struct file_reader_output {
        size_t max_battery_steps;
        size_t max_num_of_steps;
        std::shared_ptr<House> house_map;
    };

    file_reader_output readFile() const;
};

class FileWriter {
    std::string file_path;

public:
    FileWriter(const std::string& file_path);

    void writeNumberOfSteps(const Path& path);
    void writePath(const Path& path);
    void writeDirt(size_t dirt);
    void writeStatus(Status status);
};
