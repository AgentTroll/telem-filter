/**
 * @file
 */

#ifndef TELEM_FILTER_STAGE_1_PLOTTER_H
#define TELEM_FILTER_STAGE_1_PLOTTER_H

#include "telem_data_json.h"
#include "telem_staged_plotter.h"

/**
 * @brief Performs stage 1 of data processing and plots the
 * results.
 *
 * Stage 1 consists of linear interpolation of altitude
 * data and initial velocity component extraction.
 */
class stage_1_plotter : public telem_staged_plotter<first_stage_plotter> {
private:
    /**
     * The raw parsed telemetry data.
     */
    const telem_data_json &raw_data;

    /**
     * The processed telemetry data used to produce the
     * adjusted velocities.
     */
    telem_data processed_data;

public:
    /**
     * Creates a new data processor with the given input
     * parameter(s).
     *
     * @param raw_data the raw telemetry data to process
     */
    explicit stage_1_plotter(const telem_data_json &raw_data);

    /**
     * Obtains the processed raw telemetry data from this
     * stage that was used to produce the result.
     *
     * Not valid until join() returns.
     *
     * @return the processed telemetry data
     */
    [[nodiscard]] const telem_data &get_processed_data() const;

    void plotter_draw(mglGraph *gr) override;

    void plotter_calc() override;
};

#endif // TELEM_FILTER_STAGE_1_PLOTTER_H
