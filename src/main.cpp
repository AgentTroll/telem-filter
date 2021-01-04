/**
 * @file
 */

#include <mgl2/fltk.h>

#include "telem_plotter.h"
#include "telem_data_json.h"

/**
 * The main function of the program.
 *
 * @return 0 on success
 */
int main() {
    // STEP 1: Data processing
    telem_data_json raw_data{"./data/data.json"};

    csv_writer raw_data_writer{"./matlab/raw_data.csv"};
    std::map<double, double> velocities = raw_data.get_velocities();
    std::map<double, double> altitudes = raw_data.get_altitudes();
    auto v_it = velocities.cbegin();
    auto alt_it = altitudes.cbegin();
    unsigned long time_steps = velocities.size();

    // Write raw data into a CSV file, easier for MATLAB
    // understand and process
    for (int i = 0; i < time_steps; ++i, ++v_it, ++alt_it) {
        raw_data_writer << v_it->first << "," << v_it->second << "," << alt_it->second << "\n";
    }

    // STEP 2: Data plotting
    csv_writer data_1_writer{"./matlab/data_1.csv"};
    csv_reader data_2_reader{"./matlab/data_2.csv"};
    csv_writer data_3_writer{"./matlab/data_3.csv"};
    telem_plotter_data plotter_data = {
            .raw_data = raw_data,
            .data_1_writer = data_1_writer,
            .data_2_reader = data_2_reader,
            .data_3_writer = data_3_writer
    };
    telem_plotter plotter{plotter_data};
    mglFLTK mgl{&plotter, "Telemetry Filter"};

    plotter.set_wnd(&mgl);
    plotter.Run();

    return mgl.Run();
}
