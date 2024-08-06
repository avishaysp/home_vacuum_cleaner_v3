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
    void loadHouses(const std::string& houses_dir);
    std::vector<std::string> houses_files;
};

