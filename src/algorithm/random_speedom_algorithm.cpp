// random_speedom_algorithm.cpp
#include "random_speedom_algorithm.h"

std::pair<bool, Location> RandomSpeedomAlgorithm::RandomInternalHouse::minimalDistanceLocation(size_t current_battery, size_t max_steps) const {
    bool isFirst = true;
    bool isEmpty = true;
    auto min_location = current_location;
    size_t min_distance = 0;
    std::unordered_set<Location> all_minimal_location;

    for (const auto& entry : internal_graph) {
        if ((entry.second.visited && entry.second.dirt_level == 0) || (entry.first == current_location)) {
            continue;
        }
        size_t current_dist = calculateTravelDistance(entry.first);
        if(!isFeasible(current_dist, current_battery, max_steps)) {
            continue;
        }

        isEmpty = false;
        logger.log(INFO, std::format("here location: {}, current distance: {}, current battery: {}", entry.first.toString(), current_dist + 1, current_battery), FILE_LOC);

        //possible locations
        if ((entry.second.distance_from_current < min_distance) || isFirst) {
            min_distance = entry.second.distance_from_current;
            isFirst = false;
        }
    } 
    if (!isEmpty) {
        for (const auto& entry : internal_graph) {
            if ((entry.second.distance_from_current == min_distance) && (!entry.second.visited || entry.second.dirt_level != 0) && 
                isFeasible(calculateTravelDistance(entry.first), current_battery, max_steps)) {
                logger.log(INFO, std::format("RandomSpeedomAlgorithm | minimalDistanceLocation print locations: {}", entry.first.toString()), FILE_LOC);
                all_minimal_location.insert(entry.first);
            }
        }
        std::uniform_int_distribution<> dis(0, all_minimal_location.size() - 1);

        int randPos = dis(gen);
        auto it = std::next(all_minimal_location.begin(), randPos);
        min_location = *it; 
    }
    logger.log(INFO, std::format("RandomSpeedomAlgorithm | chosen location form minimalDistanceLocation: {}", min_location.toString()), FILE_LOC);
    return std::make_pair(isEmpty, min_location);
}

REGISTER_ALGORITHM(RandomSpeedomAlgorithm);




