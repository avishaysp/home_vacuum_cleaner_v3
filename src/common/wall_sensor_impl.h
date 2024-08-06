// src/common/walls_sensor_imp.h
#pragma once

#include "enums.h"
#include "WallSensor.h"
#include "../logger.h"
#include "../house.h"


class WallSensorImpl: public WallsSensor {
    std::shared_ptr<House> house;
    const Location& curr_location;

public:
    WallSensorImpl(const Location& curr_location);

    WallSensorImpl(const std::shared_ptr<House> house, const Location& curr_location);

    void setHouse(const std::shared_ptr<House> house);

	bool isWall(Direction d) const override;
};

