/**
 * @file
 */

#ifndef TELEM_FILTER_STAGE_2_PLOTTER_H
#define TELEM_FILTER_STAGE_2_PLOTTER_H

#include "stage_1_plotter.h"

/**
 * @brief Stage 2 of telemetry processing and plotting.
 *
 * This stage transforms the velocities from stage 1 using a
 * Parks-McClellan low-pass filter.
 */
class stage_2_plotter : public telem_staged_plotter<stage_1_plotter> {
private:
    /**
     * The processed telemetry data used to produce the
     * adjusted velocities from stage 1.
     */
    const telem_data &processed_data;

public:
    /**
     * Creates a new stage 2 data processor/plotter using
     * the data from the first stage.
     *
     * @param prior_stage the first stage data
     */
    explicit stage_2_plotter(stage_1_plotter &prior_stage);

    /**
     * Obtains the processed raw telemetry data from stage
     * that was used to produce the first stage result.
     *
     * Not valid until join() returns.
     *
     * @return the processed telemetry data
     */
    [[nodiscard]] const telem_data &get_processed_data() const;

    void plotter_draw(mglGraph *gr) override;

    void plotter_calc() override;
};

#endif // TELEM_FILTER_STAGE_2_PLOTTER_H
