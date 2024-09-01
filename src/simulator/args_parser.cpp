// src/simulator/args_parser.cpp
#include "args_parser.h"

ArgsParseResults ArgsParser::parse(int argc, char* argv[]) const {
    LOG(INFO, "started args parser");
    using string = std::string;
    if (argc < 1 || argc > 6) {
        writeErrorAndExit("Expected 0 to 4 arguments for the program, got {}" + std::to_string(argc - 1));
    }

    string house_path = "./";
    string algo_path = "./";
    std::optional<std::string> config_path = std::nullopt;
    size_t user_num_of_threads = 10;
    bool got_house_path = false;
    bool got_algo_path = false;
    bool got_num_of_threads = false;
    bool summary_only = false;

    const string house_prefix = "-house_path=";
    const string algo_prefix = "-algo_path=";
    const string config_prefix = "-config=";
    const string threads_prefix = "-num_threads=";
    const string summary_flag = "-summary_only";

    for (int i = 1; i < argc; i++) {
        string arg = string(argv[i]);
        if (arg.starts_with(house_prefix)) {
            if (got_house_path) {
                writeErrorAndExit("Recived houses folder argument more than once");
            }
            house_path = arg.substr(house_prefix.size());
            got_house_path = true;

        } else if (arg.starts_with(algo_prefix)) {
            if (got_algo_path) {
                writeErrorAndExit("Recived algorithms folder argument more than once");
            }
            algo_path = arg.substr(algo_prefix.size());
            got_algo_path = true;

        } else if (arg.starts_with(config_prefix)) {
            if (config_path.has_value()) {
                writeErrorAndExit("Received config file argument more than once");
            }
            config_path = arg.substr(config_prefix.size());

        } else if (arg.starts_with(threads_prefix)) {
            if (got_num_of_threads) {
                writeErrorAndExit("Recived number of threads argument more than once");
            }
            string str_arg = arg.substr(threads_prefix.size());
            if (!isOnlyDigits(str_arg)) {
                writeErrorAndExit("argument number of threads is invalid");
            }
            user_num_of_threads = std::stoul(str_arg);
            got_num_of_threads = true;

        } else if (arg == summary_flag) {
            if(summary_only) {
                writeErrorAndExit("Recived -summary_only flag more than once");
            }
            summary_only = true;
        } else {
            writeErrorAndExit("Unknown argument: " + arg);
        }
    }

    LOG(INFO, "houses folder: " + house_path);
    LOG(INFO,"algorithms folder: " + algo_path);
    LOG(INFO, "user requested number of threads: " + std::to_string(user_num_of_threads));
    LOG(INFO, "summary only: " + summary_only ? "true" : "false");
    if (config_path.has_value()) {
        LOG(INFO, "config file path: " + config_path.value());
    }
    return ArgsParseResults{house_path, algo_path, config_path, user_num_of_threads, summary_only};
}

bool ArgsParser::isOnlyDigits(const std::string& str) const {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

void ArgsParser::writeErrorAndExit(const std::string& msg) const {
    LOG(FATAL, msg);
    std::cerr << msg << std::endl;
    std::exit(EXIT_FAILURE);
}