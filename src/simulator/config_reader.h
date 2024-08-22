// src/simulator/config_reader.h
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include "../common/logger.h"


class ConfigReader {

    std::string filePath;
    std::unordered_map<std::string, std::string> configData;

public:
    explicit ConfigReader(const std::string& filePath);

    void loadFile();
    std::optional<std::string> getValue(const std::string& key) const;
};