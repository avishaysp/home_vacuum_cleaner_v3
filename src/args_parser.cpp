#include <iostream>
#include <string>
#include "args_parser.h"
#include "common/logger.h"

ArgsParseResults ArgsParser::parse(int argc, char* argv[]) const {
    using string = std::string;
    if (argc < 1 || argc > 4) {
        logger.log(FATAL, std::format("Expected 0 to 3 arguments for the program, got {}", argc - 1), FILE_LOC);
    }

    string house_path = "./";
    string algo_path = "./";
    bool summary_only = false;

    const string house_prefix = "-house_path=";
    const string algo_prefix = "-algo_path=";
    const string summary_flag = "-summary_only";

    for (int i = 1; i < argc; ++i) {
        string arg = string(argv[i]);
        if (arg.starts_with(house_prefix)) {
            house_path = arg.substr(house_prefix.size());
        } else if (arg.starts_with(algo_prefix)) {
            algo_path = arg.substr(algo_prefix.size());
        } else if (arg == summary_flag) {
            summary_only = true;
        } else {
            logger.log(FATAL, std::format("Unknown argument: {}", arg), FILE_LOC);
        }
    }

    logger.log(INFO, std::format("houses folder: {}", house_path), FILE_LOC);
    logger.log(INFO, std::format("algorithms folder: {}", algo_path), FILE_LOC);
    logger.log(INFO, std::format("summary only: {}", summary_only ? "true" : "false"), FILE_LOC);

    return ArgsParseResults{house_path, algo_path, summary_only};
}