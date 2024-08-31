// src/simulator/config_reader.cpp
#include "config_reader.h"

ConfigReader::ConfigReader(const std::string& filePath) : filePath(filePath) {}

void ConfigReader::loadFile() {
    LOG(INFO, std::format("Loading config file from path: {}", filePath));
    std::ifstream configFile(filePath);

    if (!configFile.is_open()) {
        LOG(FATAL, std::format("Unable to open config file: {}", filePath));
        std::cerr << std::format("Unable to open config file: {}", filePath) << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::string line;
    while (std::getline(configFile, line)) {
        std::istringstream lineStream(line);
        std::string key;
        if (std::getline(lineStream, key, '=')) {
            std::string value;
            if (std::getline(lineStream, value)) {
                configData[key] = value;
                LOG(INFO, std::format("found argument in config file. key: '{}', val: '{}'", key, value));
            }
        }
    }
    configFile.close();
}

std::optional<std::string> ConfigReader::getValue(const std::string& key) const {
    auto it = configData.find(key);
    if (it != configData.end()) {
        return it->second;
    }
    return std::nullopt;
}