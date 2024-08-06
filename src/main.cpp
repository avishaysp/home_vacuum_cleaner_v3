// main.cpp
#include "logger.h"
#include "io_handling.h"
// #include "simulator.h"
// #include "speedom_algorithm.h"
#include "args_parser.h"


int main(int argc, char* argv[]) {
    ArgsParser parser;
    auto args = parser.parse(argc, argv);
    std::cout << args.algos_path << " " << args.houses_path <<std::endl;
    return EXIT_SUCCESS;
}