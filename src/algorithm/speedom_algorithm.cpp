// speedom_algorithm.cpp
#include "speedom_algorithm.h"

std::pair<bool, Location> Id_208748665_206476079_SpeedomAlgorithm::DeterministicInternalHouse::minimalDistanceLocation(size_t current_battery, size_t max_steps) const {
    bool isFirst = true;
    bool isEmpty = true;
    auto min_location = current_location;
    size_t min_distance = 0;

    for (const auto& entry : internal_graph) {
        LOG(INFO, std::format("SpeedomAlgorithm | print locations: {}", entry.first.toString()));
        if ((entry.second.visited && entry.second.dirt_level == 0) || (entry.first == current_location)) {
            continue;
        }
        size_t current_dist = calculateTravelDistance(entry.first);
        if(!isFeasible(current_dist, current_battery, max_steps)) {
            continue;
        }

        isEmpty = false;

        //possible locations
        if ((entry.second.distance_from_current < min_distance) || isFirst) {
            min_location = entry.first;
            min_distance = entry.second.distance_from_current;
            isFirst = false;
        }
    }
    LOG(INFO, std::format("SpeedomAlgorithm | chosen location form minimalDistanceLocation: {}", min_location.toString()));
    return std::make_pair(isEmpty, min_location);

}

REGISTER_ALGORITHM(Id_208748665_206476079_SpeedomAlgorithm);
