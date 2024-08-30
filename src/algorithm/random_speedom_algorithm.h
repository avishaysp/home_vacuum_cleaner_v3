// random_speedom_algorithm.h
#pragma once

#include <random>
#include "abstract_speedom_algorithm.h"



class Id_208748665_206476079_RandomSpeedomAlgorithm : public AbstractSpeedomAlgorithm {

protected:

    class RandomInternalHouse : public AbstractSpeedomAlgorithm::InternalHouse {
        std::random_device rd;
        mutable std::mt19937 gen;

    public:
        RandomInternalHouse(Location& start_loc, Location& curr_loc)
            : InternalHouse(start_loc, curr_loc), gen(rd()) {}

        std::pair<bool, Location> minimalDistanceLocation(size_t current_battery, size_t max_steps) const override;
    };

    RandomInternalHouse internal_house;
    InternalHouse& getInternalHouse() override { return internal_house;} 

public:

    Id_208748665_206476079_RandomSpeedomAlgorithm() : AbstractSpeedomAlgorithm(), internal_house(starting_location, current_location) {}

};

