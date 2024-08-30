// speedom_algorithm.h
#pragma once

#include "abstract_speedom_algorithm.h"


class Id_208748665_206476079_SpeedomAlgorithm : public AbstractSpeedomAlgorithm {

protected:

    class DeterministicInternalHouse : public AbstractSpeedomAlgorithm::InternalHouse {

    public:

        DeterministicInternalHouse(Location& start_loc, Location& curr_loc)
            : InternalHouse(start_loc, curr_loc) {}

        std::pair<bool, Location> minimalDistanceLocation(size_t current_battery, size_t max_steps) const override;
    };

    DeterministicInternalHouse internal_house;

    InternalHouse& getInternalHouse() override { return internal_house; }

public:

    Id_208748665_206476079_SpeedomAlgorithm(): AbstractSpeedomAlgorithm(), internal_house(starting_location, current_location) {}

};
