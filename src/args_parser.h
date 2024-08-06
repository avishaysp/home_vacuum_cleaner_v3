#pragma once
#include <string>

struct ArgsParseResults {
    std::string houses_path;
    std::string algos_path;
    bool summary_only;
};

class ArgsParser {
    ArgsParseResults extractPaths(const std::string& input1, const std::string& input2) const;
public:
    ArgsParseResults parse(int argc, char* argv[]) const;
};