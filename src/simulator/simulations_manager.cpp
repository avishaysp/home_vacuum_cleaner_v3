// src/simulator/simulations_manager.cpp

#include "simulations_manager.h"
#include <iostream>
#include <fstream>
#include <sstream>

SimulationsManager::SimulationsManager(int argc, char* argv[]) {
    auto args = args_parser.parse(argc, argv);
    loadHouses(args.houses_path);
    loadAlgorithmLibs(args.algos_path);
    user_num_of_threads = args.user_num_of_threads;
    auto number_of_algos = AlgorithmRegistrar::getAlgorithmRegistrar().count();
    // fill scores with zeros
    scores.resize(number_of_algos);
    for (size_t i = 0; i < number_of_algos; i++) {
        scores[i].resize(houses_files.size(), 0);
    }
    if (args.summary_only) {
        Simulator::disableOutputWriting();
    }
}

void SimulationsManager::runAllSimulations() {
    logger.log(INFO, "Running all combinations concurrently", FILE_LOC);

    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();
    size_t num_of_algos = registrar.count();
    size_t num_of_houses = houses_files.size();
    size_t total_tasks = num_of_houses * num_of_algos;

    std::atomic<size_t> next_task(0);

    // for each thread, continue until task_index >= total_tasks
    auto job = [&]() {
        while (true) {
            size_t task_index = next_task.fetch_add(1);
            if (task_index >= total_tasks) {
                break;
            }

            size_t algo_index = task_index / num_of_houses;
            size_t house_index = task_index % num_of_houses;

            auto algo_iter = registrar.begin() + algo_index;
            auto algorithm = algo_iter->create();
            std::string algo_name = algo_iter->name();

            Simulator simulator;
            simulator.setAlgorithm(algorithm, algo_name);
            simulator.readHouseFile(houses_files[house_index]);
            simulator.runWithTimeout();

            scores[algo_index][house_index] = simulator.calcScore();
        }
    };

    // Launch threads
    auto hw_limit = size_t(std::thread::hardware_concurrency());
    logger.log(INFO, std::format("hardware support up to {} threads", hw_limit), FILE_LOC);
    size_t max_threads_count = std::min({user_num_of_threads, total_tasks, hw_limit});
    logger.log(INFO, std::format("launching {} threads", max_threads_count), FILE_LOC);
    std::vector<std::thread> threads;
    for (size_t i = 0; i < max_threads_count; i++) {
        logger.log(INFO, std::format("launching thread {}", i + 1), FILE_LOC);
        threads.emplace_back(job);
    }

    for (auto& tread : threads) {
        if (tread.joinable()) {
            tread.join();
        }
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

void SimulationsManager::loadAlgorithmLibs(const std::string& algorithms_dir) {
    logger.log(INFO, std::format("Loading all .so files from folder: {}", algorithms_dir), FILE_LOC);

    for (const auto& entry : std::filesystem::directory_iterator(algorithms_dir)) {
        auto path = entry.path();
        if (path.extension() == ".so") {
            logger.log(INFO, std::format("Loading algorithm file: {}", path.filename().string()), FILE_LOC);

            void* handle = dlopen(path.c_str(), RTLD_LAZY);
            if (!handle) {
                logger.log(WARNING, std::string("Failed to load algorithm: ") + path.string(), FILE_LOC);
                continue;
            } else {
                library_handles.push_back(handle);
            }
        } else {
            logger.log(INFO, std::format("Non so file: {}", path.filename().string()), FILE_LOC);
        }
    }
    logger.log(INFO, std::format("Loaded {} algorithm libraries", library_handles.size()), FILE_LOC);
}

void SimulationsManager::unloadAlgorithmLibs() {
    for (auto& handle : library_handles) {
        if (handle) {
            dlclose(handle);
        }
    }
    library_handles.clear();
    logger.log(INFO, "Unloaded all algorithm libraries", FILE_LOC);
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

SimulationsManager::~SimulationsManager() {
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    unloadAlgorithmLibs();
}
