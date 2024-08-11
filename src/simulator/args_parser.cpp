// src/simulator/args_parser.cpp
#include "args_parser.h"

ArgsParseResults ArgsParser::parse(int argc, char* argv[]) const {
    using string = std::string;
    if (argc < 1 || argc > 5) {
        logger.log(FATAL, std::format("Expected 0 to 4 arguments for the program, got {}", argc - 1), FILE_LOC);
    }

    string house_path = "./";
    string algo_path = "./";
    size_t user_num_of_threads = 10;
    bool got_house_path = false;
    bool got_algo_path = false;
    bool got_num_of_threads = false;
    bool summary_only = false;

    const string house_prefix = "-house_path=";
    const string algo_prefix = "-algo_path=";
    const string threads_prefix = "-num_threads=";
    const string summary_flag = "-summary_only";

    for (int i = 1; i < argc; i++) {
        string arg = string(argv[i]);
        if (arg.starts_with(house_prefix)) {
            if (got_house_path) {
                logger.log(FATAL, "Recived houses folder argument more than once", FILE_LOC);
            }
            house_path = arg.substr(house_prefix.size());
            got_house_path = true;
        } else if (arg.starts_with(algo_prefix)) {
            if (got_algo_path) {
                logger.log(FATAL, "Recived algorithms folder argument more than once", FILE_LOC);
            }
            algo_path = arg.substr(algo_prefix.size());
            got_algo_path = true;
        } else if (arg.starts_with(threads_prefix)) {
            if (got_num_of_threads) {
                logger.log(FATAL, "Recived number of threads argument more than once", FILE_LOC);
            }
            string str_arg = arg.substr(threads_prefix.size());
            if (!isOnlyDigits(str_arg)) {
                logger.log(FATAL, "argument number of threads is invalid", FILE_LOC);
            }
            user_num_of_threads = std::stoul(str_arg);
            got_num_of_threads = true;
        } else if (arg == summary_flag) {
            if(summary_only) {
                logger.log(FATAL, "Recived -summary_only flag more than once", FILE_LOC);
            }
            summary_only = true;
        } else {
            logger.log(FATAL, std::format("Unknown argument: {}", arg), FILE_LOC);
        }
    }

    logger.log(INFO, std::format("houses folder: {}", house_path), FILE_LOC);
    logger.log(INFO, std::format("algorithms folder: {}", algo_path), FILE_LOC);
    logger.log(INFO, std::format("summary only: {}", summary_only ? "true" : "false"), FILE_LOC);
    logger.log(INFO, std::format("user requested number of threads: {}", user_num_of_threads), FILE_LOC);

    return ArgsParseResults{house_path, algo_path, user_num_of_threads, summary_only};
}

bool ArgsParser::isOnlyDigits(const std::string& str) const {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}