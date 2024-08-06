#pragma once
#include <string>
#include <iostream>
#include <algorithm>
#include "../common/house.h"
#include "io_handling.h"
#include "../common/AbstractAlgorithm.h"
#include "../common/wall_sensor_impl.h"
#include "../common/dirt_sensor_impl.h"
#include "../common/battery_meter_impl.h"
#include "Path.h"
#include "status.h"



class Simulator {

public:

    Simulator();

    void readHouseFile(const std::string input_file_path);

    void setAlgorithm(std::unique_ptr<AbstractAlgorithm>& alg);

    void run();

    void enableVisualization();

    const Path& getPath() const;
    size_t getHistoryLength() const;

private:

    size_t battery_size;
    size_t current_battery;
    size_t max_steps;
    std::shared_ptr<House> house;
    Location current_location;
    Path history_path; // verify if needed
    WallSensorImpl walls_sensor;
    const BatteryMeterImpl battery_meter;
    DirtSensorImpl dirt_sensor;
    std::unique_ptr<AbstractAlgorithm> algo;
    size_t delta_battery;
    bool enable_live_visualization;
    std::string input_file;

    void setBatterySize(const size_t battery_size);
    void setCurrestBattery();
    void setMaxSteps(const size_t max_steps);
    void setHouse(const std::shared_ptr<House> house);
    void setCurrentLocation();
    void setWallsSensor();
    void setDirtSensor();
    void setProperties(const size_t max_num_of_steps, const size_t max_battery_steps,
                        const std::shared_ptr<House> house_map);


    void move(Step step);
    void addToHistory(Step step);
    void updateDirtLevel();

    void writeToOutputFile(Status status, std::string output_file);
    std::string addOutputPrefixToFilename(const std::string& path) const;

};
