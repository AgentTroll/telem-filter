#include "telem_data.h"

telem_data::telem_data(std::map<double, double> velocities,
                       std::map<double, double> altitudes) :
        velocities(std::move(velocities)),
        altitudes(std::move(altitudes)) {
}

const std::map<double, double> &telem_data::get_velocities() const {
    return velocities;
}

const std::map<double, double> &telem_data::get_altitudes() const {
    return altitudes;
}
