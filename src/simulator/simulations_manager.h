// src/simulations_manager.h
#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include "simulator.h"
#include "../common/AbstractAlgorithm.h"
#include "../common/AlgorithmRegistrar.h"

class SimulationsManager {
public:
    SimulationsManager(const std::string& houses_dir);

    void runAllSimulations();

private:
    std::vector<std::string> houses_files;
    std::vector<std::vector<size_t>> scores;
    void loadHouses(const std::string& houses_dir);
    void writeScoresToCSV() const;
};

