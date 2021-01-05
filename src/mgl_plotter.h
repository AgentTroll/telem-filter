/**
 * @file
 */

#ifndef TELEM_FILTER_MGL_PLOTTER_H
#define TELEM_FILTER_MGL_PLOTTER_H

#include <mutex>

#include <mgl2/wnd.h>

/**
 * @brief A MathGL drawing wrapper class to streamline
 * data processing and plotting.
 */
class mgl_plotter : public mglDraw {
private:
    /**
     * The pointer to the MathGL window which will be
     * injected post-initialization. This is needed in
     * order for the computation method to update the
     * window.
     */
    mglWnd *wnd{nullptr};

protected:
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

public:
    /**
     * Injects the plotter with the instance of the window
     * it shall update.
     *
     * @param injected_wnd the pointer to the window the
     * plotter updates
     */
    void set_wnd(mglWnd *injected_wnd);

    /**
     * Shortcut function to update the plot with new data.
     */
    void plotter_update();

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

    /**
     * Simplified Draw() function without the status code
     * return (should generally always be 0 anyways).
     *
     * @param gr the injected MathGL window to plot
     */
    virtual void plotter_draw(mglGraph *gr) = 0;

    /**
     * Overridable computation function that should be run
     * in parallel with the MathGL plot.
     */
    virtual void plotter_calc() = 0;
};

#endif // TELEM_FILTER_MGL_PLOTTER_H
