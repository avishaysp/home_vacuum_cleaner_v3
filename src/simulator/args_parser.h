// src/simulator/args_parser.h

#pragma once
#include <string>
#include <iostream>
#include "../common/logger.h"

struct ArgsParseResults {
    std::string houses_path;
    std::string algo_path;
    std::optional<std::string> config_path;
    size_t user_num_of_threads;
    bool summary_only;
};

class ArgsParser {
    bool isOnlyDigits(const std::string& str) const;
    void writeErrorAndExit(const std::string& msg) const;
public:
    ArgsParseResults parse(int argc, char* argv[]) const;
};