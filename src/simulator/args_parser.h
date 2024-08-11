// src/simulator/args_parser.h

#pragma once
#include <string>
#include <iostream>
#include "../common/logger.h"

struct ArgsParseResults {
    std::string houses_path;
    std::string algos_path;
    size_t user_num_of_threads;
    bool summary_only;
};

class ArgsParser {
    bool isOnlyDigits(const std::string& str) const;
public:
    ArgsParseResults parse(int argc, char* argv[]) const;
};