// abstract_speedom_algorithm.h
#pragma once

#include <unordered_map>
#include <vector>
#include <queue>
#include <unordered_set>
#include <optional>
#include <functional>
#include <random>
#include "../common/AbstractAlgorithm.h"
#include "../common/BatteryMeter.h"
#include "../common/WallSensor.h"
#include "../common/DirtSensor.h"
#include "../common/logger.h"
#include "../common/location.h"
#include "AlgorithmRegistration.h"



class AbstractSpeedomAlgorithm : public AbstractAlgorithm {

protected:
    // Custom hash function
    struct AlgorithmLocationHash {
        std::size_t operator()(const Location& loc) const {
            return std::hash<int>()(loc.getRow()) ^ std::hash<int>()(loc.getCol());
        }
    };

    // Custom equality function
    struct AlgorithmLocationEqual {
        bool operator()(const Location& lhs, const Location& rhs) const {
            return lhs == rhs;
        }
    };

    class InternalHouse {
    public:

        virtual ~InternalHouse() = default;  // Virtual destructor
        enum LocationType {
            DOCKING,
            CURRENT,
            CHOSEN,
        };

        static std::string locationTypetoString(LocationType type) {
            switch (type) {
                case DOCKING: return "DOCKING";
                case CURRENT: return "CURRENT";
                case CHOSEN: return "CHOSEN";
                default: return "UNKNOWN";
            }
        }

    protected:

        struct tile_stats {
            size_t dirt_level;
            bool visited;
            size_t distance_from_current;
            std::optional<Location> father_from_chosen;
            size_t distance_from_docking_station;
            std::optional<Location> father_from_docking;
            std::vector<Location> neighbors;

        };

        Location& starting_location;
        Location& current_location;


        std::unordered_map<Location, tile_stats, AlgorithmLocationHash, AlgorithmLocationEqual> internal_graph;

        size_t calculateTravelDistance(Location loc) const;
        bool reachableFromCurrent(const Location& loc, size_t current_battery, size_t max_steps, InternalHouse::LocationType start) const;

        bool isInNeighbors(const std::vector<Location>& locations,
                            Location loc) const;

        Location getNextLocationToTarget(LocationType target) const;
        bool isFeasible(size_t travel_distance, size_t current_battery, size_t max_steps) const;


    public:

        InternalHouse(Location& start_loc, Location& curr_loc)
            : starting_location(start_loc), current_location(curr_loc) {
                internal_graph[start_loc] =
                {
                    0, false, 0, std::nullopt, 0, std::nullopt, std::vector<Location>()
                };
            }

        void bfs(LocationType start, std::optional<Location> chosen_location);
        size_t getDistanceToDoc(Location other_location) const;

        virtual std::pair<bool, Location> minimalDistanceLocation(size_t current_battery, size_t max_steps) const = 0;

        std::pair<Step, Location> calcStepToTarget(LocationType target) const;

        void updateGraph(size_t dirt_level, const std::vector<Location>& possible_Locations);

        bool cleanedReachableHouse(size_t battery_size, size_t max_steps, InternalHouse::LocationType start) const;

    };

    std::size_t max_steps;

    size_t battery_size;
    Location starting_location;

    Location current_location;

    const WallsSensor* walls_sensor;
    const DirtSensor* dirt_sensor;
    const BatteryMeter* battery_meter;

    bool first_step;

    std::vector<Location> getPossibleLocations() const;

    virtual InternalHouse& getInternalHouse() = 0;

    Step updateCurrentLocAndGetNextStep(InternalHouse::LocationType target);

    Step calculateNextStep();

public:
    AbstractSpeedomAlgorithm();

    void setMaxSteps(size_t max_steps) override;
    void setWallsSensor(const WallsSensor&) override;
    void setDirtSensor(const DirtSensor&) override;
    void setBatteryMeter(const BatteryMeter&) override;
    Step nextStep() override;

};

namespace std {
    template <>
    struct hash<Location> {
        std::size_t operator()(const Location& loc) const {
            return std::hash<int>()(loc.getRow()) ^ std::hash<int>()(loc.getCol());
        }
    };
}