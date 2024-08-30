#pragma once

#include <gtest/gtest.h>
#include "../src/simulator/simulator.h"
#include "../src/simulator/house.h"
#include "../src/common/location.h"
#include "../src/simulator/Path.h"

class SimulatorTest : public ::testing::Test {
protected:
    void SetUp() override {
        house = std::make_shared<House>(3,3);
        for(size_t i=0; i<house->getRowsCount(); i++){
            for(size_t j=0; j<house->getColsCount(); j++){
                if (i == 1 && j == 1){
                    house->getTile(i, j).setAsWall();
                    continue;
                } else if (i == 2 && j == 1) {
                    house->getTile(i, j).setAsDockingStation();
                    continue;
                }
                house->getTile(i, j).setDirtLevel(i + 1);
            }
        }
        simulator.setProperties(10, 5, house);
        simulator.curr_status = Status::DEAD;
        simulator.current_location = Location(0, 0);
    }

    Location moveSimulator(Step step) {
        simulator.move(step);
        return simulator.current_location;
    }

    size_t updateDirt() {
        simulator.updateDirtLevel();
        return house->getTile(simulator.current_location).getDirtLevel();
    }

    Simulator simulator;
    std::shared_ptr<House> house;
};



