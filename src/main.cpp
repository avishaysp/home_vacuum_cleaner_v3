// main.cpp
#include "logger.h"
#include "io_handling.h"
#include "simulator.h"
#include "speedom_algorithm.h"
#include "args_parser.h"


int main(int argc, char* argv[]) {
    ArgsParser parser;
    auto args = parser.parse(argc, argv);
    Simulator simulator;
    simulator.readHouseFile(args.input_file_path);
    if (args.enable_visualization) {
        simulator.enableVisualization();
    }
    simulator.setAlgorithm(std::make_shared<SpeedomAlgorithm>());
    simulator.run();

    return EXIT_SUCCESS;
}