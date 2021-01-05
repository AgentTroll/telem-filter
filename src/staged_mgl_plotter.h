/**
 * @file
 */
#ifndef TELEM_FILTER_STAGED_MGL_PLOTTER_H
#define TELEM_FILTER_STAGED_MGL_PLOTTER_H

#include "mgl_plotter.h"

/**
 * @brief Indicator type for the first stage of a staged
 * plotter.
 */
struct first_stage_plotter {
};

/**
 * The instance of the first_stage_plotter used as a dummy
 * prior stage value.
 */
static first_stage_plotter fsp_inst;

/**
 * @brief Superclass for staged processing/plotting, in
 * which data is processed in stages and in different
 * contexts.
 *
 * @tparam prior_stage_type the type of the prior stage
 * used to collect data for further processing
 */
template<typename prior_stage_type>
class staged_mgl_plotter : public mgl_plotter {
private:
    /**
     * Latch used to determine whether this plotter's
     * calculation function has finished and exited.
     */
    c11_binary_latch latch;

protected:
    /**
     * The data from the prior stage.
     */
    prior_stage_type &prior_stage;

public:
    /**
     * Creates a new instance of a staged plotter with the
     * given data from a prior stage.
     *
     * @param prior_stage the data for the prior stage
     */
    explicit staged_mgl_plotter(prior_stage_type &prior_stage);

    /**
     * Creates a new instance of staged plotter for the
     * first stage.
     */
    staged_mgl_plotter();

    /**
     * Blocks the calling thread until this plotter's
     * calculation method completes and exits.
     */
    void join();

    /**
     * Calculation function that delegates to
     * plotter_calc() and then releases the latch when
     * the calculation exits.
     */
    void Calc() override;
};

template<typename prior_stage_type>
staged_mgl_plotter<prior_stage_type>::staged_mgl_plotter(prior_stage_type &prior_stage) :
        prior_stage(prior_stage) {
}

template<typename prior_stage_type>
staged_mgl_plotter<prior_stage_type>::staged_mgl_plotter() :
        staged_mgl_plotter(fsp_inst) {
}

template<typename prior_stage_type>
void staged_mgl_plotter<prior_stage_type>::join() {
    latch.wait();
}

template<typename prior_stage_type>
void staged_mgl_plotter<prior_stage_type>::Calc() {
    plotter_calc();

    latch.release();
}

#endif // TELEM_FILTER_STAGED_MGL_PLOTTER_H
