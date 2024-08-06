// src/simulations_manager.cpp

#include "simulations_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>

SimulationsManager::SimulationsManager(const std::string& houses_dir) {
    loadHouses(houses_dir);
    auto number_of_algos = AlgorithmRegistrar::getAlgorithmRegistrar().count();
    // fill scores with zeros
    scores.resize(number_of_algos);
    for (size_t i = 0; i < number_of_algos; i++) {
        scores[i].resize(houses_files.size(), 0);
    }
}

void SimulationsManager::runAllSimulations() {
    logger.log(INFO, "running all combinations", FILE_LOC);
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    logger.log(INFO, std::format("found {} algoritms registered", registrar.count()), FILE_LOC);

    size_t algo_index = 0;
    for (const auto& algo : registrar) {
        size_t house_index = 0;
        for (const auto& house_file : houses_files) {
            auto algorithm = algo.create();
            Simulator simulator;
            simulator.setAlgorithm(algorithm, algo.name());
            simulator.readHouseFile(house_file);
            simulator.run();
            scores[algo_index][house_index] = simulator.getScore();
            house_index++;
        }
        algo_index++;
    }
    writeScoresToCSV();
}

void SimulationsManager::loadHouses(const std::string& houses_dir) {
    logger.log(INFO, std::format("searching for .house files in folder: {}", houses_dir.size()), FILE_LOC);

    for (const auto& entry : std::filesystem::directory_iterator(houses_dir)) {
        auto path = entry.path();
        if (path.extension() == ".house") {
            logger.log(INFO, std::format("found house file: {}", path.filename().string()), FILE_LOC);
            houses_files.push_back(path);
        }
    }
    logger.log(INFO, std::format("found total of {} house files in relevant dir", houses_files.size()), FILE_LOC);
}



void SimulationsManager::writeScoresToCSV() const {
    std::string csv_path = "summary.csv";
    std::ofstream file(csv_path);

    if (!file.is_open()) {
        logger.log(FATAL, "Failed to open file: " + csv_path, FILE_LOC);
        return;
    }

    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();

    // Write the header
    file << "Algorithm\\House";
    for (const auto& house_file : houses_files) {
        std::size_t lastSlashPos = house_file.find_last_of('/');
        std::string house_name = (lastSlashPos == std::string::npos) ? house_file : house_file.substr(lastSlashPos + 1);
        std::size_t lastDotPos = house_name.find_last_of('.');
        if (lastDotPos != std::string::npos) {
            house_name = house_name.substr(0, lastDotPos);
        }
        file << "," << house_name;
    }
    file << "\n";

    // Write the scores
    size_t algo_index = 0;
    for (const auto& algo : registrar) {
        file << algo.name();
        for (const auto& score : scores[algo_index]) {
            file << "," << score;
        }
        file << "\n";
        algo_index++;
    }

    file.close();
    logger.log(INFO, "Scores written to " + csv_path, FILE_LOC);
}
