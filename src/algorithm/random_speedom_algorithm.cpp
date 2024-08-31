// random_speedom_algorithm.cpp
#include "random_speedom_algorithm.h"

std::pair<bool, Location> Id_208748665_206476079_RandomSpeedomAlgorithm::RandomInternalHouse::minimalDistanceLocation(size_t current_battery, size_t max_steps) const {
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
        LOG_INFO(std::format("here location: {}, current distance: {}, current battery: {}", entry.first.toString(), current_dist + 1, current_battery));

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
                LOG_INFO(std::format("RandomSpeedomAlgorithm | minimalDistanceLocation print locations: {}", entry.first.toString()));
                all_minimal_location.insert(entry.first);
            }
        }
        std::uniform_int_distribution<> dis(0, all_minimal_location.size() - 1);

        int randPos = dis(gen);
        auto it = std::next(all_minimal_location.begin(), randPos);
        min_location = *it;
    }
    LOG_INFO(std::format("RandomSpeedomAlgorithm | chosen location form minimalDistanceLocation: {}", min_location.toString()));
    return std::make_pair(isEmpty, min_location);
}

REGISTER_ALGORITHM(Id_208748665_206476079_RandomSpeedomAlgorithm);




