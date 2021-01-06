#include "stage_3_plotter.h"

stage_3_plotter::stage_3_plotter(stage_2_plotter &prior_stage) :
        staged_telem_plotter<stage_2_plotter>(prior_stage) {
}

void stage_3_plotter::plotter_draw(mglGraph *gr) {
    std::scoped_lock<std::mutex> lock{data_mutex};

    gr->Clf();

    gr->SubPlot(2, 2, 0);
    gr->Title("Time vs. v_x");
    gr->Label('x', "Time (s)");
    gr->Label('y', "Velocity (m/s)");
    gr->Grid();
    gr->Box();
    gr->SetRanges(data.SubData(0), data.SubData(1));
    gr->Axis("xy");
    gr->Plot(data.SubData(0), data.SubData(1), "r-");

    gr->SubPlot(2, 2, 1);
    gr->Title("Time vs. v_y");
    gr->Label('x', "Time (s)");
    gr->Label('y', "Velocity (m/s)");
    gr->Grid();
    gr->Box();
    gr->SetRanges(data.SubData(0), data.SubData(2));
    gr->Axis("xy");
    gr->Plot(data.SubData(0), data.SubData(2), "r-");

    gr->SubPlot(2, 2, 2);
    gr->Title("Time vs. Velocity Error");
    gr->Label('x', "Time (s)");
    gr->Label('y', "Velocity Error (m/s)");
    gr->Grid();
    gr->Box();
    gr->SetRanges(data.SubData(0), data.SubData(3));
    gr->Axis("xy");
    gr->Plot(data.SubData(0), data.SubData(3), "r-");

    gr->SubPlot(2, 2, 3);
    gr->Title("Time vs. Altitude Error");
    gr->Label('x', "Time (s)");
    gr->Label('y', "Altitude Error (km)");
    gr->Grid();
    gr->Box();
    gr->SetRanges(data.SubData(0), data.SubData(4));
    gr->Axis("xy");
    gr->Plot(data.SubData(0), data.SubData(4));
}

void stage_3_plotter::plotter_calc() {
    data.Create(5, 1);

    // Collect data from prior stage
    prior_stage.join();
    const std::map<double, vector2d> &v_stage_2 = prior_stage.get_result();

    const telem_data &processed_data = prior_stage.get_processed_data();
    const std::map<double, double> &velocities = processed_data.get_velocities();
    const std::map<double, double> &altitudes = processed_data.get_altitudes();

    // Adjustment loop
    auto v_it = velocities.cbegin();
    auto alt_it = altitudes.cbegin();
    auto v_f_it = v_stage_2.cbegin();

    double last_t = 0;
    double v_y_a_integral = 0;

    int time_steps = v_stage_2.size();
    for (int i = 0; i < time_steps; ++i, ++v_it, ++alt_it, ++v_f_it) {
        double t = v_it->first;
        double v = v_it->second;
        double alt = alt_it->second;
        const vector2d &v_f = v_f_it->second;

        // Adjust v_x
        double v_sq = v * v;
        double v_y_f = v_f.get_y();
        double v_x_adjusted = std::sqrt(v_sq - std::min(v_y_f * v_y_f, v_sq));

        vector2d v_adjusted{v_x_adjusted, v_y_f};
        result[t] = v_adjusted;

        double dt = t - last_t;
        last_t = t;

        v_y_a_integral += v_y_f * dt / 1000;

        // Record data to the matrix
        {
            std::scoped_lock<std::mutex> lock{data_mutex};

            // Expand the data matrix for new data
            if (i != 0) {
                data.Insert('y', i);
            }

            // 0: Time
            data.Put(t, 0, i);

            // 1: Velocity X
            data.Put(v_adjusted.get_x(), 1, i);
            // 2: Velocity Y
            data.Put(v_adjusted.get_y(), 2, i);

            // 3: Velocity Error
            data.Put(v_adjusted.mag() - v, 3, i);
            // 4: Altitude Error
            data.Put(v_y_a_integral - alt, 4, i);
        }

        Check();
        plotter_update();
    }
}
