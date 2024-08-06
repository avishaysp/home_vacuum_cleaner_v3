// src/simulations_manager.cpp

#include "simulations_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>

SimulationsManager::SimulationsManager(const std::string& houses_dir) {
    loadHouses(houses_dir);
}

void SimulationsManager::runAllSimulations() {
    logger.log(INFO, "running all combinations", FILE_LOC);
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    logger.log(INFO, std::format("found {} algoritms registered", registrar.count()), FILE_LOC);

    for (const auto& algo : registrar) {
        for (const auto& house_file : houses_files) {
            auto algorithm = algo.create();
            Simulator simulator;
            simulator.setAlgorithm(algorithm, algo.name());
            simulator.readHouseFile(house_file);
            simulator.run();
            std::string output_file = "output_" + house_file + "_" + algo.name() + ".txt";
            std::cout << output_file << std::endl;
            // write output
        }
    }
}

void SimulationsManager::loadHouses(const std::string& houses_dir) {

    for (const auto& entry : std::filesystem::directory_iterator(houses_dir)) {
        auto path = entry.path();
        if (path.extension() == ".house") {
            logger.log(INFO, std::format("found house file: {}", path.filename().string()), FILE_LOC);
            houses_files.push_back(path);
        }
    }
    logger.log(INFO, std::format("found total of {} house files in relevant dir", houses_files.size()), FILE_LOC);
}
