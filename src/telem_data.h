/**
 * @file
 */

#ifndef TELEM_FILTER_TELEM_DATA_H
#define TELEM_FILTER_TELEM_DATA_H

#include <map>
#include <string>

/**
 * @brief Represents telemetry data that consists of
 * velocity magnitude and altitude values.
 */
class telem_data {
protected:
    /**
     * A mapping of time offsets to velocity magnitudes.
     */
    std::map<double, double> velocities;

    /**
     * A mapping of time offsets to altitudes.
     */
    std::map<double, double> altitudes;

public:
    /**
     * Initializes the telemetry data with the given
     * mappings of time to velocities and altitudes.
     *
     * @param velocities the mapping of time to velocities
     * @param altitudes the mapping of time to altitudes
     */
    telem_data(std::map<double, double> velocities,
               std::map<double, double> altitudes);

    /**
     * Obtains the mapping of time offsets to velocity
     * magnitudes.
     *
     * @return the time to velocity magnitude map
     */
    [[nodiscard]] const std::map<double, double> &get_velocities() const;

    /**
     * Obtains the mapping of time offsets to altitude.
     *
     * @return the time to altitude map
     */
    [[nodiscard]] const std::map<double, double> &get_altitudes() const;
};

#endif // TELEM_FILTER_TELEM_DATA_H
