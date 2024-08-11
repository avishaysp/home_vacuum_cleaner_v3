// src/common/dirt_sensor_imp.h
#pragma once

#include "DirtSensor.h"
#include "logger.h"
#include "../simulator/house.h"


class DirtSensorImpl: public DirtSensor {

	std::shared_ptr<House> house;
    Location& curr_location;

public:

    DirtSensorImpl(Location& curr_location);
    DirtSensorImpl(const std::shared_ptr<House> house, Location& curr_location);
    void setHouse(const std::shared_ptr<House> house);
    int dirtLevel() const override;
};

