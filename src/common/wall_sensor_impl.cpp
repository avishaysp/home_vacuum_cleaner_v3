// src/common/walls_sensor_imp.cpp
#include "wall_sensor_impl.h"

WallSensorImpl::WallSensorImpl(const Location& curr_location) : house(nullptr), curr_location(curr_location) {}

WallSensorImpl::WallSensorImpl(const std::shared_ptr<House> house, const Location& curr_location) : house(house), curr_location(curr_location) {}

void WallSensorImpl::setHouse(const std::shared_ptr<House> house) {
    this->house = house;
}

bool WallSensorImpl::isWall(Direction d) const {
    int row = curr_location.getRow();
    int col = curr_location.getCol();
    int row_limit = house->getRowsCount();
    int col_limit = house->getColsCount();
    switch (d)
    {
    case Direction::North:
        return (row == 0) || (house->getTile(row - 1, col).isWall());
        break;
    case Direction::South:
        return (row == row_limit - 1) || (house->getTile(row + 1, col).isWall());
        break;
    case Direction::East:
        return (col == col_limit - 1) || (house->getTile(row, col + 1).isWall());
        break;
    case Direction::West:
        return (col == 0) || (house->getTile(row, col - 1).isWall());
        break;
    default:
        break;
    }
    return false;
}