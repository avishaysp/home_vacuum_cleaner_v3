#pragma once
#include <string>

struct ArgsParseResults {
    std::string input_file_path;
    bool enable_visualization;
};

class ArgsParser {
public:
    ArgsParseResults parse(int argc, char* argv[]);
};