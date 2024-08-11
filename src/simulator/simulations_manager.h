// src/simulator/simulations_manager.h
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <dlfcn.h>
#include <thread>
#include <atomic>
#include "simulator.h"
#include "args_parser.h"
#include "../common/AbstractAlgorithm.h"
#include "../common/AlgorithmRegistrar.h"

class SimulationsManager {
public:
    SimulationsManager(int argc, char* argv[]);
    ~SimulationsManager();
    SimulationsManager(const SimulationsManager&) = delete;
    SimulationsManager& operator=(const SimulationsManager&) = delete;

    void runAllSimulations();

private:
    ArgsParser args_parser;
    size_t user_num_of_threads;
    std::vector<std::string> houses_files;
    std::vector<std::string> algos_files;
    std::vector<std::vector<size_t>> scores;
    std::vector<void*> library_handles;
    void loadHouses(const std::string& houses_dir);
    void loadAlgorithmLibs(const std::string& algos_dir);
    void unloadAlgorithmLibs();
    void writeScoresToCSV() const;
    std::string getHouseName(const std::string& house_file) const;
};

