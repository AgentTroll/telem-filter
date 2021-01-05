/**
 * @file
 */

#ifndef TELEM_FILTER_TELEM_DATA_JSON_H
#define TELEM_FILTER_TELEM_DATA_JSON_H

#include "telem_data.h"

/**
 * @brief This class represents the data file containing
 * the JSON telemetry data extracted using SpaceXtract.
 */
class telem_data_json : public telem_data {
public:
    /**
     * Initializes the telemetry data by reading from the
     * data file at the given path.
     *
     * @param file_path the path to the file containing
     * telemetry data
     * @throws std::invalid_argument if the path to the
     * file is not valid
     */
    explicit telem_data_json(const std::string &file_path);
};

#endif // TELEM_FILTER_TELEM_DATA_JSON_H
