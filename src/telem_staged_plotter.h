/**
 * @file
 */

#ifndef TELEM_FILTER_TELEM_STAGED_PLOTTER_H
#define TELEM_FILTER_TELEM_STAGED_PLOTTER_H

#include <map>

#include "staged_mgl_plotter.h"
#include "vector2d.h"

/**
 * @brief A superclass of plotter stages used for this
 * project to process velocity data.
 *
 * @tparam prior_stage_type the type of the prior plotter
 * stage used to obtain data for further processing
 */
template<typename prior_stage_type>
class telem_staged_plotter : public staged_mgl_plotter<prior_stage_type> {
protected:
    /**
     * Map containing the result of this data processing
     * stage.
     */
    std::map<double, vector2d> result{};

public:
    /**
     * Super constructor to the staged_mgl_plotter next
     * stage constructor.
     *
     * @param prior_stage the prior stage data
     */
    explicit telem_staged_plotter(prior_stage_type &prior_stage);

    /**
     * Super constructor to the staged_mgl_plotter first
     * stage constructor.
     */
    telem_staged_plotter();

    /**
     * Obtains the data result after this stage of data
     * processing.
     *
     * Not valid until join() returns.
     *
     * @return the result map
     */
    [[nodiscard]] const std::map<double, vector2d> &get_result() const;
};

template<typename prior_stage_type>
telem_staged_plotter<prior_stage_type>::telem_staged_plotter(prior_stage_type &prior_stage):
        staged_mgl_plotter<prior_stage_type>(prior_stage) {
}

template<typename prior_stage_type>
telem_staged_plotter<prior_stage_type>::telem_staged_plotter() = default;

template<typename prior_stage_type>
const std::map<double, vector2d> &telem_staged_plotter<prior_stage_type>::get_result() const {
    return result;
}

#endif // TELEM_FILTER_TELEM_STAGED_PLOTTER_H
