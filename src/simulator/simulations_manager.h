// src/simulator/simulations_manager.h
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <dlfcn.h>
#include <thread>
#include <atomic>
#include <mutex>
#include "simulator.h"
#include "args_parser.h"
#include "../common/AbstractAlgorithm.h"
#include "../common/AlgorithmRegistrar.h"
#include "config_reader.h"

class SimulationsManager {
public:
    SimulationsManager(int argc, char* argv[]);
    ~SimulationsManager();
    SimulationsManager(const SimulationsManager&) = delete;
    SimulationsManager& operator=(const SimulationsManager&) = delete;

    void runAllSimulations();

private:
    ArgsParser args_parser;
    std::string houses_path;
    std::string algo_path;
    size_t user_num_of_threads;
    std::vector<std::string> houses_files;
    std::vector<std::string> algos_files;
    std::vector<std::vector<size_t>> scores;
    std::vector<void*> library_handles;
    std::mutex error_file_mutex;

    void initializeSettings(const ArgsParseResults& args);
    void loadHouses(const std::string& houses_dir);
    void loadAlgorithmLibs(const std::string& algos_dir);
    void unloadAlgorithmLibs();
    void launchThreads(std::function<void()> job, size_t total_tasks);
    void writeScoresToCSV() const;
    std::string getHouseName(const std::string& house_file) const;
    void logErrorToFile(const std::string& algo_name, const std::string& error_message);
};

