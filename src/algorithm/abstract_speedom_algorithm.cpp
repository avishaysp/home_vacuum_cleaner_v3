// random_speedom_algorithm.cpp
#include "abstract_speedom_algorithm.h"

AbstractSpeedomAlgorithm::AbstractSpeedomAlgorithm() :
    max_steps(0),
    battery_size(0),
    starting_location(Location()),
    current_location(starting_location),
    walls_sensor(),
    dirt_sensor(),
    battery_meter(),
    first_step(true)
    {}

void AbstractSpeedomAlgorithm::setMaxSteps(std::size_t maxSteps) {
    max_steps = maxSteps;
}

void AbstractSpeedomAlgorithm::setWallsSensor(const WallsSensor& wallsSensor) {
    walls_sensor = &wallsSensor;
}

void AbstractSpeedomAlgorithm::setDirtSensor(const DirtSensor& dirtSensor) {
    dirt_sensor = &dirtSensor;
}

void AbstractSpeedomAlgorithm::setBatteryMeter(const BatteryMeter& batteryMeter) {
    battery_meter = &batteryMeter;
}


Step AbstractSpeedomAlgorithm::updateCurrentLocAndGetNextStep(InternalHouse::LocationType target) {
    auto& house = getInternalHouse();
    auto [step, next_location] = house.calcStepToTarget(target);
    current_location = next_location;
    return step;
}

Step AbstractSpeedomAlgorithm::calculateNextStep() {
    auto& house = getInternalHouse();
    LOG(INFO, "AbstractSpeedomAlgorithm | nextStep");
    std::vector<Location> possibleLocations = getPossibleLocations();
    size_t battery_level = battery_meter->getBatteryState();
    size_t dirt_level = current_location == starting_location ? 0 : dirt_sensor->dirtLevel();

    if (first_step) {
        battery_size = battery_level;
        first_step = false;
    }
    house.updateGraph(dirt_level, possibleLocations);

    if (current_location == starting_location && battery_level < battery_size) {
        if (house.cleanedReachableHouse(battery_size, max_steps, InternalHouse::DOCKING)) {
            return Step::Finish;
        }
        return Step::Stay;
    }

    house.bfs(InternalHouse::DOCKING, std::nullopt);

    if (current_location == starting_location) {
        auto [isEmpty, chosen_location] = house.minimalDistanceLocation(battery_level, max_steps);
        if (isEmpty) {
            return Step::Finish;
        }
        house.bfs(InternalHouse::CHOSEN, chosen_location);
        return updateCurrentLocAndGetNextStep(InternalHouse::CHOSEN);
    }

    if (dirt_level) {
        if (std::min(battery_level, max_steps) > house.getDistanceToDoc(current_location)) {
            return Step::Stay;
        }

        return updateCurrentLocAndGetNextStep(InternalHouse::DOCKING);
    }

    house.bfs(InternalHouse::CURRENT, std::nullopt);

    if (house.cleanedReachableHouse(battery_level, max_steps, InternalHouse::CURRENT)) {
        return updateCurrentLocAndGetNextStep(InternalHouse::DOCKING);
    }

    auto [isEmpty, chosen_location] = house.minimalDistanceLocation(battery_level, max_steps);
    if (isEmpty) {
        return updateCurrentLocAndGetNextStep(InternalHouse::DOCKING);
    }
    house.bfs(InternalHouse::CHOSEN, chosen_location);
    return updateCurrentLocAndGetNextStep(InternalHouse::CHOSEN);
}


Step AbstractSpeedomAlgorithm::nextStep() {
    Step next_step = calculateNextStep();
    if (next_step != Step::Finish) {
        max_steps--;
    }

    return next_step;
}



std::vector<Location> AbstractSpeedomAlgorithm::getPossibleLocations() const {
    LOG(INFO, "AbstractSpeedomAlgorithm | getPossibleLocations");
    std::vector<Location> possible_Locations;
    int curr_row = current_location.getRow();
    int curr_col = current_location.getCol();
    if (!walls_sensor->isWall(Direction::North)){
        possible_Locations.push_back(Location(curr_row - 1, curr_col));
    }
    if (!walls_sensor->isWall(Direction::South)){
        possible_Locations.push_back(Location(curr_row + 1, curr_col));
    }
    if (!walls_sensor->isWall(Direction::East)){
        possible_Locations.push_back(Location(curr_row, curr_col + 1));
    }
    if (!walls_sensor->isWall(Direction::West)){
        possible_Locations.push_back(Location(curr_row, curr_col - 1));
    }
    return possible_Locations;
}

void AbstractSpeedomAlgorithm::InternalHouse::bfs(LocationType start, std::optional<Location> chosen_location) {

    std::queue<Location> q;
    std::unordered_set<Location> bfsVisited;

    if (start == DOCKING) {
        q.push(starting_location);
        bfsVisited.insert(starting_location);
        internal_graph.at(starting_location).distance_from_docking_station = 0;
        internal_graph.at(starting_location).father_from_docking = std::nullopt;
    }

    else if (start == CURRENT) {
        q.push(current_location);
        bfsVisited.insert(current_location);
        internal_graph.at(current_location).distance_from_current = 0;
    }

    else {
        q.push(chosen_location.value());
        bfsVisited.insert(chosen_location.value());
    }

    while (!q.empty()) {
        Location current = q.front();
        q.pop();

        for (Location neighbor : internal_graph.at(current).neighbors) {
            if (bfsVisited.find(neighbor) == bfsVisited.end()) {
                if (start == DOCKING) {
                    internal_graph.at(neighbor).distance_from_docking_station = internal_graph.at(current).distance_from_docking_station + 1;
                    internal_graph.at(neighbor).father_from_docking = current;
                }

                else if (start == CURRENT) {
                    internal_graph.at(neighbor).distance_from_current = internal_graph.at(current).distance_from_current + 1;
                }

                else {
                    internal_graph.at(neighbor).father_from_chosen = current;
                }

                q.push(neighbor);
                bfsVisited.insert(neighbor);
            }
        }
    }
}


size_t AbstractSpeedomAlgorithm::InternalHouse::getDistanceToDoc(Location other_location) const {
    LOG(INFO, "AbstractSpeedomAlgorithm | getDistanceToDoc");
    return internal_graph.at(other_location).distance_from_docking_station;
}

Location AbstractSpeedomAlgorithm::InternalHouse::getNextLocationToTarget(InternalHouse::LocationType target) const {
    return target == DOCKING ?
        internal_graph.at(current_location).father_from_docking.value() : internal_graph.at(current_location).father_from_chosen.value();
}

std::pair<Step, Location> AbstractSpeedomAlgorithm::InternalHouse::calcStepToTarget(InternalHouse::LocationType target) const {
    Location next = getNextLocationToTarget(target);
    if (next.getRow() > current_location.getRow()){
        return std::make_pair(Step::South, next);
    }
    if (next.getRow() < current_location.getRow()){
        return std::make_pair(Step::North, next);
    }
    if (next.getCol() < current_location.getCol()){
        return std::make_pair(Step::West, next);
    }
    return std::make_pair(Step::East, next);

}

void AbstractSpeedomAlgorithm::InternalHouse::updateGraph(size_t dirt_level, const std::vector<Location>& possible_Locations) {
    LOG(INFO, "AbstractSpeedomAlgorithm | updateGraph");
    internal_graph.at(current_location).dirt_level = dirt_level;
    if(internal_graph.at(current_location).visited) {
        return;
    }
    LOG(INFO, "Speedom Algorithm | after verifying visited");
    internal_graph.at(current_location).visited = true;
    internal_graph.at(current_location).neighbors = possible_Locations;
    for (auto& loc : possible_Locations) {
        if (!internal_graph.contains(loc)){
            internal_graph[loc] = {0, false, 0, std::nullopt, 0, std::nullopt, std::vector<Location>()};
        }
        if (!isInNeighbors(internal_graph.at(loc).neighbors, current_location)) {
            internal_graph.at(loc).neighbors.push_back(current_location);
        }
    }
}

std::pair<bool, Location> AbstractSpeedomAlgorithm::InternalHouse::minimalDistanceLocation(size_t, size_t) const {
    // Empty definition as this function is meant to be overridden
    return std::make_pair(false, Location());
}


bool AbstractSpeedomAlgorithm::InternalHouse::isInNeighbors(const std::vector<Location>& locations, Location loc) const {

    return (std::find(locations.begin(), locations.end(), loc) != locations.end());
}

size_t AbstractSpeedomAlgorithm::InternalHouse::calculateTravelDistance(Location loc) const {
    if (current_location == starting_location) {
        return 2 * internal_graph.at(loc).distance_from_docking_station;
    }

    return internal_graph.at(loc).distance_from_docking_station + internal_graph.at(loc).distance_from_current;


}


bool AbstractSpeedomAlgorithm::InternalHouse::cleanedReachableHouse(size_t current_battery, size_t max_steps, InternalHouse::LocationType start) const {
    for (auto& pair : internal_graph) {
        if (pair.second.visited && !pair.second.dirt_level) {
            continue;
        }
        if (!reachableFromCurrent(pair.first, current_battery, max_steps, start)) {
            continue;
        }
        return false;
    }
    return true;
}

bool AbstractSpeedomAlgorithm::InternalHouse::reachableFromCurrent(const Location& loc, size_t current_battery, size_t max_steps, InternalHouse::LocationType start) const {
    if (start == DOCKING){
        return (1 + 2 * internal_graph.at(loc).distance_from_docking_station) <= std::min(current_battery, max_steps);
    }
    return (1 + internal_graph.at(loc).distance_from_docking_station + internal_graph.at(loc).distance_from_current) <= std::min(current_battery, max_steps);
}

bool AbstractSpeedomAlgorithm::InternalHouse::isFeasible(size_t travel_distance, size_t current_battery, size_t max_steps) const {
    return travel_distance + 1 <= std::min(current_battery, max_steps);
}

