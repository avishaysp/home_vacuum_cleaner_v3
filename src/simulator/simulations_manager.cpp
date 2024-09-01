// src/simulator/simulations_manager.cpp

#include "simulations_manager.h"


SimulationsManager::SimulationsManager(int argc, char* argv[]) {
    auto args = args_parser.parse(argc, argv);
    initializeSettings(args);
    loadHouses(houses_path);
    loadAlgorithmLibs(algo_path);
    auto number_of_algos = AlgorithmRegistrar::getAlgorithmRegistrar().count();
    // fill scores with zeros
    scores.resize(number_of_algos);
    for (size_t i = 0; i < number_of_algos; i++) {
        scores[i].resize(houses_files.size(), 0);
    }
}

void SimulationsManager::initializeSettings(const ArgsParseResults& args) {

    houses_path = args.houses_path;
    algo_path = args.algo_path;
    user_num_of_threads =  args.user_num_of_threads;
    bool summary_only = args.summary_only;

    if (!args.config_path.has_value()) {
        LOG(INFO, "config path was not provided");
        return;
    }

    ConfigReader configReader(*args.config_path);
    configReader.loadFile();
    if (auto ret = configReader.getValue("house_path"); ret.has_value()) {
        houses_path = ret.value();
    }
    if (auto ret = configReader.getValue("algo_path"); ret.has_value()) {
        algo_path = ret.value();
    }
    if (auto ret = configReader.getValue("num_threads"); ret.has_value()) {
        auto val = ret.value();
        if (!val.empty() && std::all_of(val.begin(), val.end(), ::isdigit)) {
            user_num_of_threads =  std::stoul(val);
        }
    }
    if (auto ret = configReader.getValue("summary_only"); ret.has_value()) {
        summary_only = ret.value() == "true";
    }


    if (summary_only) {
        Simulator::disableOutputWriting();
    }
}


void SimulationsManager::runAllSimulations() {
    LOG(INFO, "Running all combinations concurrently");

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
            std::string house_name = getHouseName(houses_files[house_index]);
            try {
                std::string algo_name = algo_iter->name();
                Simulator simulator;
                logger.setLogFileName(house_name + "-" + algo_name + ".log");
                simulator.readHouseFile(houses_files[house_index]);
                simulator.setAlgorithm(algorithm, algo_name);
                scores[algo_index][house_index] = simulator.timeoutScore();
                simulator.runWithTimeout();

                scores[algo_index][house_index] = simulator.calcScore();
            }

            catch(const AlgorithmException& e) {
                logErrorToFile(algo_iter->name(), e.what());
            }

            catch(const std::exception& e) {
                logErrorToFile(house_name, e.what());
            }
        }
    };

    launchThreads(job, total_tasks);
    writeScoresToCSV();
    LOG(INFO, "Run Done!");
}

void SimulationsManager::launchThreads(std::function<void()> job, size_t total_tasks) {
    auto hw_limit = size_t(std::thread::hardware_concurrency());
    size_t max_threads_count = std::min({user_num_of_threads, total_tasks, hw_limit});
    std::vector<std::thread> threads;
    for (size_t i = 0; i < max_threads_count; i++) {
        threads.emplace_back(job);
    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void SimulationsManager::loadHouses(const std::string& houses_dir) {

    for (const auto& entry : std::filesystem::directory_iterator(houses_dir)) {
        auto path = entry.path();
        if (path.extension() == ".house") {
            houses_files.push_back(path);
        }
    }
}

void SimulationsManager::loadAlgorithmLibs(const std::string& algorithms_dir) {
    auto& registrar = AlgorithmRegistrar::getAlgorithmRegistrar();

    for (const auto& entry : std::filesystem::directory_iterator(algorithms_dir)) {
        auto path = entry.path();
        if (path.extension() == ".so") {
            auto curr_cnt = registrar.count();
            void* handle = dlopen(path.c_str(), RTLD_LAZY);
            if (!handle) {
                std::string error_message = dlerror();
                LOG(FATAL, "Failed to load algorithm: " + path.string() + " Error: " + error_message);
                logErrorToFile(path.filename().string(), "Failed to load algorithm: " + path.string());
                continue;
            } else if (registrar.count() != curr_cnt + 1) {
                LOG(FATAL, "New algoritm did not raise the counter in the registrar");
                logErrorToFile(path.filename().string(), "New algoritm did not raise the counter in the registrar");
                continue;
            } else {
                library_handles.push_back(handle);
            }
        } else {
            LOG(INFO, "found a filw with ext that is not .so: {}");
        }
    }
}

void SimulationsManager::unloadAlgorithmLibs() {
    for (auto& handle : library_handles) {
        if (handle) {
            dlclose(handle);
        }
    }
    library_handles.clear();
    LOG(INFO, "Unloaded all algorithm libraries");
}


void SimulationsManager::writeScoresToCSV() const {
    std::string csv_path = "summary.csv";
    std::ofstream file(csv_path);

    if (!file.is_open()) {
        LOG(FATAL, "Failed to open file: " + csv_path);
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
    LOG(INFO, "Scores written to " + csv_path);
}

SimulationsManager::~SimulationsManager() {
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    unloadAlgorithmLibs();
}

std::string SimulationsManager::getHouseName(const std::string& house_file) const {
    std::size_t last_slash_pos = house_file.find_last_of('/');
    auto house_name = house_file.substr(last_slash_pos + 1);
    std::size_t last_dot_pos = house_name.find_last_of('.');
    if (last_dot_pos != std::string::npos) {
        house_name = house_name.substr(0, last_dot_pos);
    }
    return house_name;
}

void SimulationsManager::logErrorToFile(const std::string& name, const std::string& error_message) {
    std::lock_guard<std::mutex> lock(error_file_mutex);
    std::string error_file_name = name + ".error";
    std::ofstream error_file(error_file_name, std::ios::app);
    if (error_file.is_open()) {
        error_file << error_message << std::endl;
        error_file.close();
    } else {
        std::cerr << "Failed to open error file: " << error_file_name << std::endl;
    }
}