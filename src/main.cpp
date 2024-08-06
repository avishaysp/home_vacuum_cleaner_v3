// main.cpp
#include "common/logger.h"
// #include "simulator.h"
// #include "speedom_algorithm.h"
#include "args_parser.h"
#include "simulator/simulations_manager.h"


int main(int argc, char* argv[]) {
    ArgsParser parser;
    auto args = parser.parse(argc, argv);
    SimulationsManager simulations_manager(args.houses_path, args.algos_path);
    simulations_manager.runAllSimulations();
    return EXIT_SUCCESS;
}