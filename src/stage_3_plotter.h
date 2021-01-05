/**
 * @file
 */

#ifndef TELEM_FILTER_STAGE_3_PLOTTER_H
#define TELEM_FILTER_STAGE_3_PLOTTER_H

#include "stage_2_plotter.h"

/**
 * @brief Stage 3 of data processing/plotting.
 *
 * This stage adjusts the data from stage 2 to account for
 * velocity error.
 */
class stage_3_plotter : public telem_staged_plotter<stage_2_plotter> {
public:
    /**
     * Creates a new processor/plotter stage with the data
     * from stage 2 of the processing.
     *
     * @param prior_stage the stage 2 processor data
     */
    explicit stage_3_plotter(stage_2_plotter &prior_stage);

    void plotter_draw(mglGraph *gr) override;

    void plotter_calc() override;
};

#endif // TELEM_FILTER_STAGE_3_PLOTTER_H
