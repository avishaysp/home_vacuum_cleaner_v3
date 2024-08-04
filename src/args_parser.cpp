#include <iostream>
#include <string>
#include "args_parser.h"
#include "logger.h"

ArgsParseResults ArgsParser::parse(int argc, char* argv[]) const {
    using string = std::string;
    if (argc != 3) {
        logger.log(FATAL, std::format("Expected 2 arguments for the program got {}", argc - 1));
    }
    string arg1 = string(argv[1]);
    string arg2 = string(argv[2]);

    return extractPaths(arg1, arg2);
}

ArgsParseResults ArgsParser::extractPaths(const std::string& input1, const std::string& input2) const {
    std::string housePath;
    std::string algoPath;

    const std::string housePrefix = "-house_path=";
    const std::string algoPrefix = "-algo_path=";

    // parse input1
    if (input1.find(housePrefix) == 0) {
        housePath = input1.substr(housePrefix.size());
    } else if (input1.find(algoPrefix) == 0) {
        algoPath = input1.substr(algoPrefix.size());
    }

    // parse input2
    if (input2.find(housePrefix) == 0) {
        housePath = input2.substr(housePrefix.size());
    } else if (input2.find(algoPrefix) == 0) {
        algoPath = input2.substr(algoPrefix.size());
    }
    logger.log(INFO, std::format("houses path: {}", housePath));
    logger.log(INFO, std::format("algorithms path: {}", algoPath));
    return ArgsParseResults{housePath, algoPath};
}