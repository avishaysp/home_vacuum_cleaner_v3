// io_handling.cpp
#include "io_handling.h"


std::vector<std::string> FileReader::split(const std::string &str, const char delimiter) const {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }

    return result;
}

size_t FileReader::readArgument(const std::string &str) const {
    std::vector<std::string> splits = split(str, '=');
    if (splits.size() != 2) {
        logger.log(FATAL, "Invalid format: " + str, FILE_LOC);
    }

    std::string str_arg = trim(splits[1]);
    return strToSize_t(str_arg);
}

size_t FileReader::strToSize_t(const std::string &str) const {
    try {
        size_t res = std::stoull(str);
        return res;
    } catch (const std::invalid_argument& e) {
        logger.log(FATAL, "Invalid argument: " + std::string(e.what()), FILE_LOC);
    } catch (const std::out_of_range& e) {
        logger.log(FATAL, "Out of range: " + std::string(e.what()), FILE_LOC);
    }
    return 0;
}

std::string FileReader::trim(const std::string &str) const {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
        ++start;
    }

    auto end = str.end();
    do {
        --end;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
}

Location FileReader::parseLocation(const std::string &str) const {
    auto tmp_vec = split(str, '|');
    return Location(
        strToSize_t(tmp_vec[0].substr(1)),                            // row
        strToSize_t(tmp_vec[1].substr(0, tmp_vec[1].size() - 1))      // col
    );
}


void FileReader::ParseHouse(std::ifstream &file, std::shared_ptr<House> house) const {
    if (!file.is_open()) {
        logger.log(FATAL, "error reading house from file", FILE_LOC);
    }
    size_t num_of_rows = house->getRowsCount();
    size_t num_of_cols = house->getColsCount();
    std::string line;
    size_t row_index = 0;
    while (std::getline(file, line) && row_index < num_of_rows) {
        std::vector<char> vec_line(line.begin(), line.end());
        size_t width = vec_line.size() < num_of_cols ? vec_line.size() : num_of_cols;
        for (size_t col_index = 0; col_index < width; col_index++) {
            char c  = vec_line[col_index];
            if (c == 'D') {
                house->getTile(row_index, col_index).setAsDockingStation();
                house->setDockingStation(Location(row_index, col_index));
                logger.log(INFO, std::format("Set a Docking Station ({},{})", row_index, col_index), FILE_LOC);
            } else if (c == 'W') {
                house->getTile(row_index, col_index).setAsWall();
                logger.log(INFO, std::format("ParseHouse Set a Wall ({},{})", row_index, col_index), FILE_LOC);
            } else if (c == ' ') {
                house->getTile(row_index, col_index).setDirtLevel(0);
                logger.log(INFO, std::format("Set an empty Tile ({},{})", row_index, col_index), FILE_LOC);
            } else if (isdigit(c)) {
                house->getTile(row_index, col_index).setDirtLevel(int(c - '0'));
                logger.log(INFO, std::format("Set an dirty Tile ({},{}). Dirt level: {}", row_index, col_index, int(c - '0')), FILE_LOC);
            } else {
                logger.log(FATAL, std::format("Invalid charecter in house map ({},{})", row_index, col_index), FILE_LOC);
            }
        }
        row_index++;
    }
    logger.log(INFO, std::format("Populated house of size {} by {}",num_of_rows, num_of_cols), FILE_LOC);
}


FileReader::FileReader(const std::string& file_path) : file_path(file_path) {}


FileReader::file_reader_output FileReader::readFile() const {
    size_t max_num_of_steps = 0;
    size_t max_battery_steps = 0;
    size_t rows_count = 0;
    size_t cols_count = 0;

    std::ifstream file(file_path);

    if (!file.is_open()) {
        logger.log(FATAL, std::format("Failed to open the file: {}. error: {}", file_path, std::strerror(errno)), FILE_LOC);
    }

    std::string line;

    if (!std::getline(file, line)) {
        logger.log(FATAL, "Failed to read first line of input file", FILE_LOC);
    }

    if (std::getline(file, line)) {
        max_num_of_steps = readArgument(line);
    } else {
        logger.log(FATAL, "Failed to read 2nd line with MaxSteps", FILE_LOC);
    }

    if (std::getline(file, line)) {
        max_battery_steps = readArgument(line);
    } else {
        logger.log(FATAL, "Failed to read 3rd line with MaxBattery", FILE_LOC);
    }

    if (std::getline(file, line)) {
        rows_count = readArgument(line);
    } else {
        logger.log(FATAL, "Failed to read 4th line with Rows", FILE_LOC);
    }

    if (std::getline(file, line)) {
        cols_count = readArgument(line);
    } else {
        logger.log(FATAL, "Failed to read 5th line with Cols", FILE_LOC);
    }

    std::shared_ptr<House> house = std::make_shared<House>(rows_count, cols_count);
    ParseHouse(file, house);
    file.close();

    return {max_battery_steps, max_num_of_steps, house};
}

FileWriter::FileWriter(const std::string& file_path) : file_path(file_path) {
    std::ofstream file(file_path);
    file.close();
}

void FileWriter::writeNumberOfSteps(int number_of_steps) {
    std::ofstream file(file_path, std::ios_base::app);
    if (!file.is_open()) {
        std::cout << "Could not open file for writing" << std::endl;
    }
    file << "NumSteps:" << number_of_steps << std::endl;

    file.close();
}

void FileWriter::writeDirt(size_t dirt) {
    std::ofstream file(file_path, std::ios_base::app);
    if (!file.is_open()) {
        std::cout << "Could not open file for writing" << std::endl;
    }
    file << "DirtLeft:" << dirt << std::endl;
    file.close();
}

void FileWriter::writeStatus(Status status) {
    std::ofstream file(file_path, std::ios_base::app);
    if (!file.is_open()) {
        std::cout << "Could not open file for writing" << std::endl;
    }
    switch (status)
    {
    case Status::FINISH:
        file << "FINISHED";
        break;
    case Status::DEAD:
        file << "DEAD";
        break;
    default:
        file << "WORKING";
        break;
    }
    file << std::endl;
    file.close();
}

void FileWriter::writeInDock(bool in_dock) {
    std::ofstream file(file_path, std::ios_base::app);
    if (!file.is_open()) {
        std::cout << "Could not open file for writing" << std::endl;
    }
    file << "InDock:" << (in_dock ? "TRUE" : "FALSE") << std::endl;
    file.close();
}

void FileWriter::writeScore(int score) {
    std::ofstream file(file_path, std::ios_base::app);
    if (!file.is_open()) {
        std::cout << "Could not open file for writing" << std::endl;
    }
    file << "Score:" << score << std::endl;
    file.close();
}

void FileWriter::writePath(const Path& path) {
    std::ofstream file(file_path, std::ios_base::app);
    if (!file.is_open()) {
        std::cout << "Could not open file for writing" << std::endl;
    }
    file << path.toString();

    file.close();
}



