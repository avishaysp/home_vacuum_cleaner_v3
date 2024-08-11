// main.cpp

#include "simulator/simulations_manager.h"


int main(int argc, char* argv[]) {
    SimulationsManager simulations_manager(argc, argv);
    simulations_manager.runAllSimulations();
    return EXIT_SUCCESS;
}