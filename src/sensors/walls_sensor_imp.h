#pragma once

#include "../skelaton/enums.h"
#include "../skelaton/walls_sensor.h"
#include "../logger.h"
#include "../house.h"


class WallsSensorImp: public WallsSensor {
    std::shared_ptr<House> house;
    const Location& curr_location;

public:
    WallsSensorImp(const Location& curr_location);

    WallsSensorImp(const std::shared_ptr<House> house, const Location& curr_location);

    void setHouse(const std::shared_ptr<House> house);

	bool isWall(Direction d) const override;
};

