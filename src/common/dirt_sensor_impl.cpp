// src/common/dirt_sensor_imp.cpp
#include "dirt_sensor_impl.h"

DirtSensorImpl::DirtSensorImpl(Location& curr_location) :  house(nullptr), curr_location(curr_location) { }

DirtSensorImpl::DirtSensorImpl(const std::shared_ptr<House> house, Location& curr_location) :  house(house), curr_location(curr_location) { }

void DirtSensorImpl::setHouse(const std::shared_ptr<House> house) {
    this->house = house;
}

int DirtSensorImpl::dirtLevel() const {
    return house->getTile(curr_location).getDirtLevel();
}

