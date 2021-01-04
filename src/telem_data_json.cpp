#include "telem_data_json.h"

#include <fstream>

#include <nlohmann/json.hpp>

telem_data_json::telem_data_json(const std::string &file_path) :
        telem_data({}, {}) {
    std::ifstream telem_file{file_path};
    if (!telem_file.good()) {
        telem_file.close();
        throw std::invalid_argument{"File could not be opened."};
    }

    std::string line;
    while (std::getline(telem_file, line)) {
        const auto &json = nlohmann::json::parse(line);
        double t = json["time"];
        double velocity = json["velocity"];
        double altitude = json["altitude"];

        velocities[t] = velocity;
        altitudes[t] = altitude;
    }

    telem_file.close();
}
