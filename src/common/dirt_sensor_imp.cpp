// src/common/dirt_sensor_imp.cpp
#include "dirt_sensor_imp.h"

DirtSensorImp::DirtSensorImp(Location& curr_location) :  house(nullptr), curr_location(curr_location) { }

DirtSensorImp::DirtSensorImp(const std::shared_ptr<House> house, Location& curr_location) :  house(house), curr_location(curr_location) { }

void DirtSensorImp::setHouse(const std::shared_ptr<House> house) {
    this->house = house;
}

int DirtSensorImp::dirtLevel() const {
    return house->getTile(curr_location).getDirtLevel();
}

