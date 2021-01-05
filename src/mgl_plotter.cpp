#include "mgl_plotter.h"

#include <FL/Fl.H>

void mgl_plotter::set_wnd(mglWnd *injected_wnd) {
    wnd = injected_wnd;
}

/**
 * Window update helper function to be called using
 * Fl::awake().
 *
 * @param wnd_void_ptr the injected pointer to the window
 * that shall be updated
 */
static void update_wnd(void *wnd_void_ptr) {
    auto *wnd = static_cast<mglWnd *>(wnd_void_ptr);
    wnd->Update();
}

void mgl_plotter::plotter_update() {
    Fl::awake(update_wnd, wnd);
}

void mgl_plotter::Calc() {
    plotter_calc();
}

int mgl_plotter::Draw(mglGraph *gr) {
    plotter_draw(gr);

    return 0;
}
