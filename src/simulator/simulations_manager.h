// src/simulations_manager.h
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <dlfcn.h>
#include "simulator.h"
#include "../common/AbstractAlgorithm.h"
#include "../common/AlgorithmRegistrar.h"

class SimulationsManager {
public:
    SimulationsManager(const std::string& houses_dir, const std::string& algo_dir, bool summary_only);
    ~SimulationsManager();
    SimulationsManager(const SimulationsManager&) = delete;
    SimulationsManager& operator=(const SimulationsManager&) = delete;

    void runAllSimulations();

private:
    std::vector<std::string> houses_files;
    std::vector<std::string> algos_files;
    std::vector<std::vector<size_t>> scores;
    std::vector<void*> library_handles;
    void loadHouses(const std::string& houses_dir);
    void loadAlgorithmLibs(const std::string& algos_dir);
    void unloadAlgorithmLibs();
    void writeScoresToCSV() const;
};

