#include "simulator.h"
#include "live_simulator.h"

bool Simulator::write_output = true;

Simulator::Simulator() :
    score(0),
    battery_size(0),
    current_battery(0),
    max_steps(0),
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
    algo_name("")
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

void Simulator::writeToOutputFile(Status status) {
    std::string output_file = outputFileName();
    FileWriter fw(output_file);
    fw.writeNumberOfSteps(history_path);
    fw.writeDirt(house->calcTotalDirt());
    fw.writeStatus(status);
    fw.writePath(history_path);
}


//setters

void Simulator::setBatterySize(const size_t battery_size) {
    this->battery_size = battery_size * 100;
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
    std::size_t lastSlashPos = house_file.find_last_of('/');

    std::string directory = (lastSlashPos == std::string::npos) ? "" : house_file.substr(0, lastSlashPos + 1);
    std::string filename = (lastSlashPos == std::string::npos) ? house_file : house_file.substr(lastSlashPos + 1);
    std::size_t lastDotPos = filename.find_last_of('.');
    if (lastDotPos != std::string::npos) {
        filename = filename.substr(0, lastDotPos);
    }

    std::string output_file = std::format("{}-{}.txt", filename, algo_name);

    return directory + output_file;
}

void Simulator::run() {
    Step step;
    Status final_status = Status::WORKING;
    logger.log(INFO, std::format("running algorithm '{}' on house {}", algo_name, house_file), FILE_LOC);
    for (size_t i = 0; i < max_steps; ++i) {

        if ((current_location != house->getDockingStation()) && current_battery <= 0) {
            logger.log(WARNING, "Battery level is empty, Can not continue cleaning", FILE_LOC);
            final_status = Status::DEAD;
            addToHistory(step);
            break;
        }

        step = algo->nextStep();

        if ((step == Step::Stay) && (current_location == house->getDockingStation())) {
            if (current_battery == battery_size) {
                logger.log(WARNING, "Stayed in docking station even though battary is full. Inappropriate behavior.", FILE_LOC);
            }
            current_battery = std::min(current_battery + delta_battery, battery_size);
            logger.log(INFO, std::format("New battery after charging {}", current_battery / 100), FILE_LOC);
        }

        else if (step == Step::Stay) {
            logger.log(INFO, "Stay and clean", FILE_LOC);
            updateDirtLevel();
            current_battery -= 100;
        }

        else if (step != Step::Finish) {
            move(step);
            current_battery -= 100;
        }

        else {
            logger.log(INFO, "Simulator successfully finished running ", FILE_LOC);
            if (enable_live_visualization) {
                live_simulator.simulate(*house, current_location, step, false, (max_steps - 1) - i, current_battery / 100);
            }
            final_status = Status::FINISH;
            addToHistory(step);
            break;
        }

        addToHistory(step);
        if (enable_live_visualization) {
            live_simulator.simulate(*house, current_location, step, current_location == house->getDockingStation(), (max_steps - 1) - i, current_battery / 100);
        }
    }

    if (final_status != Status::FINISH && current_location == house->getDockingStation() && algo->nextStep() == Step::Finish) {
        final_status = Status::FINISH;
        addToHistory(Step::Finish);
    }
    if (write_output) {
        logger.log(INFO, "Prepering output file", FILE_LOC);
        std::string output_file = outputFileName();
        writeToOutputFile(final_status);
    }
}

size_t Simulator::getScore() const {
    return score;
}

void Simulator::updateDirtLevel() {
    if (house->getTile(current_location).getDirtLevel() == 0) {
        logger.log(FATAL, "Stayed in a floor tile that is already clean. Inappropriate behavior.", FILE_LOC);
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
                logger.log(FATAL, "Tried to move North from northest row", FILE_LOC);
            }
            next_loc = Location(next_row, curr_col);
            break;
        case Step::South:
            next_row = curr_row + 1;
            if (next_row >= (int)rows) {
                logger.log(FATAL, "Tried to move South from southest row", FILE_LOC);
            }
            next_loc = Location(next_row, curr_col);
            break;
        case Step::East:
            next_col = curr_col + 1;
            if (next_col >= (int)cols) {
                logger.log(FATAL, "Tried to move East from most east col", FILE_LOC);
            }
            next_loc = Location(curr_row, next_col);
            break;
        case Step::West:
            next_col = curr_col - 1;
            if (next_col < 0) {
                logger.log(FATAL, "Tried to move West from most west col", FILE_LOC);
            }
            next_loc = Location(curr_row, next_col);
            break;
        default:
            break;
    }
    if (house->getTile(next_loc).isWall()) {
        logger.log(FATAL, "Tried to move into a wall", FILE_LOC);
    }
    logger.log(INFO, std::format("Move to location {}", next_loc.toString()), FILE_LOC);
    current_location = next_loc;
}






