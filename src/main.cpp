// main.cpp

#include "simulator/simulations_manager.h"


int main(int argc, char* argv[]) {
    logger.setLogFileName("main_thread.log");
    SimulationsManager simulations_manager(argc, argv);
    simulations_manager.runAllSimulations();
    return EXIT_SUCCESS;
}