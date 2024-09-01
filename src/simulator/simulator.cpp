// src/simulator/simulator.cpp

#include "simulator.h"
#include "live_simulator.h"

bool Simulator::write_output = true;

Simulator::Simulator() :
    battery_size(0),
    current_battery(0),
    max_steps(0),
    steps_cnt(0),
    house(nullptr),
    current_location(),
    history_path(),
    walls_sensor(current_location),
    battery_meter(current_battery),
    dirt_sensor(current_location),
    algo(nullptr),
    delta_battery(0),
    enable_live_visualization(false),
    house_file(""),
    algo_name(""),
    initial_dirt_level(0),
    curr_status(Status::WORKING)
    {}

void Simulator::readHouseFile(const std::string input_file_path) {
    house_file = input_file_path;
    FileReader fr(input_file_path);
    FileReader::file_reader_output args = fr.readFile();
    setProperties(args.max_num_of_steps, args.max_battery_steps, args.house_map);
}

void Simulator::disableOutputWriting() {
    write_output = false;
}

void Simulator::writeToOutputFile(Status status) const {
    std::string output_file = outputFileName();
    FileWriter fw(output_file);
    fw.writeNumberOfSteps(steps_cnt);
    fw.writeDirt(house->calcTotalDirt());
    fw.writeStatus(status);
    fw.writeInDock(current_location == house->getDockingStation());
    fw.writeScore(calcScore());
    fw.writePath(history_path);
}


//setters

void Simulator::setBatterySize(const size_t battery_size) {
    this->battery_size = battery_size * BATTERY_FACTOR;
}

void Simulator::setCurrestBattery() {
    current_battery = battery_size;
}

void Simulator::setMaxSteps(const size_t max_steps) {
    this->max_steps = max_steps;
}

void Simulator::setHouse(const std::shared_ptr<House> house) {
    this->house = house;
}

//Todo
void Simulator::setCurrentLocation() {
    this->current_location = house->getDockingStation();
}

void Simulator::setWallsSensor() {
    walls_sensor.setHouse(house);
}

void Simulator::setDirtSensor() {
    dirt_sensor.setHouse(house);
}

void Simulator::enableVisualization() {
    enable_live_visualization = true;
}

void Simulator::setAlgorithm(std::unique_ptr<AbstractAlgorithm>& alg, std::string algo_name) {
    alg->setWallsSensor(walls_sensor);
    alg->setDirtSensor(dirt_sensor);
    alg->setBatteryMeter(battery_meter);
    alg->setMaxSteps(max_steps);
    algo = std::move(alg);
    this->algo_name = algo_name;
}

void Simulator::setProperties(const size_t max_num_of_steps, const size_t max_battery_steps,
                        const std::shared_ptr<House> house_map) {

    setBatterySize(max_battery_steps);
    setCurrestBattery();
    setMaxSteps(max_num_of_steps);
    setHouse(house_map);
    setCurrentLocation();
    setWallsSensor();
    setDirtSensor();
    delta_battery = battery_size / 20;
    initial_dirt_level = house->calcTotalDirt();
}

void Simulator::addToHistory(Step step) {
    history_path.addEntry(step);
}

const Path& Simulator::getPath() const {
    return history_path;
}

size_t Simulator::getHistoryLength() const {
    return history_path.getLength();
}

std::string Simulator::outputFileName() const {
    std::string house_name = getHouseName();
    auto directory = getDirectory();
    std::string output_file = house_name + "-" + algo_name + ".txt";

    return directory + output_file;
}

std::string Simulator::getHouseName() const {
    std::size_t last_slash_pos = house_file.find_last_of('/');
    auto house_name = house_file.substr(last_slash_pos + 1);
    std::size_t last_dot_pos = house_name.find_last_of('.');
    if (last_dot_pos != std::string::npos) {
        house_name = house_name.substr(0, last_dot_pos);
    }
    return house_name;
}

std::string Simulator::getDirectory() const {
    std::size_t last_slash_pos = house_file.find_last_of('/');

    return (last_slash_pos == std::string::npos) ? "" : house_file.substr(0, last_slash_pos + 1);
}

void Simulator::runWithTimeout() {
    auto start_time = std::chrono::steady_clock::now();
    Step step;

    for (size_t i = 0; i < max_steps; ++i) {
        auto current_time = std::chrono::steady_clock::now();
        auto elapsed_time = std::chrono::duration_cast<std::chrono::microseconds>(current_time - start_time);
        // if (size_t(elapsed_time.count()) > TIMEOUT_CONST * max_steps) {
        //     LOG(WARNING, "Time limit exceeded, ending simulation early");
        //     curr_status = Status::TIMEOUT;
        //     break;
        // }

        if ((current_location != house->getDockingStation()) && current_battery <= 0) {
            LOG(WARNING, "Battery level is empty, Can not continue cleaning");
            curr_status = Status::DEAD;
            break;
        }

        try {
            step = algo->nextStep();
        } catch(const std::exception& e) {
            throwAlgorithmException(e.what());
            return;
        }

        if (step != Step::Finish) { steps_cnt++; }

        if ((step == Step::Stay) && (current_location == house->getDockingStation())) {
            if (current_battery == battery_size) {
                LOG(WARNING, "Stayed in docking station even though battary is full. Inappropriate behavior.");
            }
            current_battery = std::min(current_battery + delta_battery, battery_size);
        }

        else if (step == Step::Stay) {
            LOG(INFO, "Stay and clean");
            updateDirtLevel();
            current_battery -= BATTERY_FACTOR;
        }

        else if (step != Step::Finish) {
            move(step);
            current_battery -= BATTERY_FACTOR;
        }

        else {
            LOG(INFO, "Simulator successfully finished running ");
            if (enable_live_visualization) {
                live_simulator.simulate(*house, current_location, step, false, (max_steps - 1) - i, current_battery / BATTERY_FACTOR);
            }
            curr_status = Status::FINISH;
            addToHistory(step);
            break;
        }

        addToHistory(step);
        if (enable_live_visualization) {
            live_simulator.simulate(*house, current_location, step, current_location == house->getDockingStation(), (max_steps - 1) - i, current_battery / BATTERY_FACTOR);
        }
    }

    if (curr_status != Status::TIMEOUT && curr_status != Status::FINISH && current_location == house->getDockingStation() && algo->nextStep() == Step::Finish) {
        curr_status = Status::FINISH;
        addToHistory(Step::Finish);
    }
    writeOutputIfEnabled();
}

void Simulator::writeOutputIfEnabled() const {
    if (!write_output) {
        return;
    }
    LOG(INFO, "Prepering output file");
    std::string output_file = outputFileName();
    writeToOutputFile(curr_status);
}

size_t Simulator::timeoutScore() const {
    return max_steps * 2 + initial_dirt_level * 300 + 2000;
}

size_t Simulator::calcScore() const {
    size_t dirt_left = house->calcTotalDirt();
    bool robot_in_dock = current_location == house->getDockingStation();

    if (curr_status == Status::TIMEOUT) {
        return max_steps * 2 + initial_dirt_level * 300 + 2000;
    }
    if (curr_status == Status::DEAD) {
        return max_steps + dirt_left * 300 + 2000;
    }
    if (curr_status == Status::FINISH && !robot_in_dock) {
        return max_steps + dirt_left * 300 + 3000;
    }
    return steps_cnt + dirt_left * 300 + (robot_in_dock ? 0 : 1000);
}

void Simulator::updateDirtLevel() {
    if (house->getTile(current_location).getDirtLevel() == 0) {
        throwAlgorithmException("Stayed in a floor tile that is already clean. Inappropriate behavior.");
    }
    (house->getTile(current_location)).decreaseOneDirt();
}

void Simulator::move(Step step) {
    size_t rows = house->getRowsCount();
    size_t cols = house->getColsCount();
    size_t curr_row = current_location.getRow();
    size_t curr_col = current_location.getCol();
    Location next_loc;
    int next_row = 0;
    int next_col = 0;
    switch (step) {
        case Step::North:
            next_row = curr_row - 1;
            if (next_row < 0) {
                throwAlgorithmException("Tried to move North from northest row");
            }
            next_loc = Location(next_row, curr_col);
            break;
        case Step::South:
            next_row = curr_row + 1;
            if (next_row >= (int)rows) {
                throwAlgorithmException("Tried to move South from southest row");
            }
            next_loc = Location(next_row, curr_col);
            break;
        case Step::East:
            next_col = curr_col + 1;
            if (next_col >= (int)cols) {
                throwAlgorithmException("Tried to move East from most east col");

            }
            next_loc = Location(curr_row, next_col);
            break;
        case Step::West:
            next_col = curr_col - 1;
            if (next_col < 0) {
                throwAlgorithmException("Tried to move West from most west col");
            }
            next_loc = Location(curr_row, next_col);
            break;
        default:
            break;
    }
    if (house->getTile(next_loc).isWall()) {
        throwAlgorithmException("Tried to move into a wall");
    }
    current_location = next_loc;
}

void Simulator::throwAlgorithmException(const std::string& msg) const {
    LOG(FATAL, msg);
    writeOutputIfEnabled();
    throw AlgorithmException(msg);
}




