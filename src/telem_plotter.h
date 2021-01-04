/**
 * @file
 */

#ifndef TELEM_FILTER_TELEM_PLOTTER_H
#define TELEM_FILTER_TELEM_PLOTTER_H

#include <mutex>

#include <mgl2/wnd.h>

#include "csv_reader.h"
#include "csv_writer.h"
#include "telem_data.h"

/**
 * @brief A struct which contains the input parameters for
 * the plotter class.
 */
struct telem_plotter_data {
    /**
     * The raw telemetry data to process.
     */
    telem_data &raw_data;
    /**
     * Data output after step 1 of the processing.
     */
    csv_writer &data_1_writer;
    /**
     * Data input after step 2 of data processing.
     */
    csv_reader &data_2_reader;
    /**
     * Data output after step 3 of data processing.
     */
    csv_writer &data_3_writer;
};

/**
 * @brief Processing and plotting functions to be passed to
 * MathGL for execution.
 */
class telem_plotter : public mglDraw {
private:
    /**
     * The input data that will be processed into MathGL
     * format to be plotted.
     */
    const telem_plotter_data &plotter_data;
    /**
     * The data processed through the Calc() method and
     * used to plot on the MathGL window.
     *
     * Because this is accessed between threads that
     * perform computations and window updates, it is
     * protected by the data_mutex member.
     */
    mglData data;
    /**
     * The mutex used to protect accesses to the data
     * member.
     */
    std::mutex data_mutex;

    /**
     * The pointer to the MathGL window which will be
     * injected post-initialization. This is needed in
     * order for the computation method to update the
     * window.
     */
    mglWnd *wnd{nullptr};

public:
    /**
     * Initializes the MathGL telemetry plotter with the
     * given input parameters.
     *
     * @param plotter_data the data input to plot
     */
    explicit telem_plotter(const telem_plotter_data &plotter_data);

    /**
     * Injects the plotter with the instance of the window
     * it shall update.
     *
     * @param injected_wnd the pointer to the window the plotter
     * updates
     */
    void set_wnd(mglWnd *injected_wnd);

    /**
     * The drawing function, which will plot the data
     * points in a MathGL window.
     *
     * @param gr the injected MathGL window
     * @return 0 if successful
     */
    int Draw(mglGraph *gr) override;

    /**
     * The computation function. This performs the
     * computation in parallel with the draw function and
     * updates the plot(s) accordingly.
     */
    void Calc() override;
};

#endif // TELEM_FILTER_TELEM_PLOTTER_H
