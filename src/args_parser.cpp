#include <iostream>
#include <string>
#include "args_parser.h"
#include "logger.h"

ArgsParseResults ArgsParser::parse(int argc, char* argv[]) {
    bool enable_visualization = false;
    if (argc < 2) {
        logger.log(FATAL, "requires at least one argument - input file, got 0");
    } else if (argc == 3) {
        if (std::string(argv[2]) != "--visualize") {
            logger.log(FATAL, std::format("Unknown argument: {}", std::string(argv[2])));
        } else {
            enable_visualization = true;
        }
    }
    std::string input_file_path = argv[1];
    return ArgsParseResults{input_file_path, enable_visualization};
}
