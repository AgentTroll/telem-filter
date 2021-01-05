/**
 * @file
 */

#include <mgl2/fltk.h>

#include "stage_3_plotter.h"

/**
 * The main function of the program.
 *
 * @return 0 on success
 */
int main() {
    telem_data_json raw_data{"./data/data.json"};

    stage_1_plotter stage_1{raw_data};
    stage_2_plotter stage_2{stage_1};
    stage_3_plotter stage_3{stage_2};

    mglFLTK mgl_stage_1{&stage_1, "Stage 1"};
    mglFLTK mgl_stage_2{&stage_2, "Stage 2"};
    mglFLTK mgl_stage_3{&stage_3, "Stage 3"};

    stage_1.set_wnd(&mgl_stage_1);
    stage_2.set_wnd(&mgl_stage_2);
    stage_3.set_wnd(&mgl_stage_3);

    stage_1.Run();
    stage_2.Run();
    stage_3.Run();

    return mgl_fltk_run();
}
