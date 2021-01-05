#include "stage_2_plotter.h"

#include "digital_filter.h"

/**
 * Parks-McClellan FIR coefficients:
 *   - 0-1 Hz break frequencies
 *   - 0.001 ripple deviation
 *   - LPF [1 0] coefficients
 *   - Sampled at ~30 Hz
 *
 * Filter coefficients generated with MATLAB.
 */
static const std::vector<double> PM_LPF_COEFFS = {
        0.0001, 0.0001, 0.0001, 0.0001, 0.0002, 0.0003, 0.0003, 0.0004, 0.0006, 0.0007, 0.0009, 0.0011,
        0.0013, 0.0016, 0.0019, 0.0022, 0.0026, 0.0030, 0.0035, 0.0040, 0.0045, 0.0051, 0.0058, 0.0065,
        0.0072, 0.0079, 0.0087, 0.0096, 0.0104, 0.0113, 0.0123, 0.0132, 0.0141, 0.0151, 0.0160, 0.0169,
        0.0178, 0.0187, 0.0195, 0.0203, 0.0211, 0.0218, 0.0224, 0.0230, 0.0235, 0.0239, 0.0243, 0.0246,
        0.0247, 0.0248, 0.0248, 0.0247, 0.0246, 0.0243, 0.0239, 0.0235, 0.0230, 0.0224, 0.0218, 0.0211,
        0.0203, 0.0195, 0.0187, 0.0178, 0.0169, 0.0160, 0.0151, 0.0141, 0.0132, 0.0123, 0.0113, 0.0104,
        0.0096, 0.0087, 0.0079, 0.0072, 0.0065, 0.0058, 0.0051, 0.0045, 0.0040, 0.0035, 0.0030, 0.0026,
        0.0022, 0.0019, 0.0016, 0.0013, 0.0011, 0.0009, 0.0007, 0.0006, 0.0004, 0.0003, 0.0003, 0.0002,
        0.0001, 0.0001, 0.0001, 0.0001
};

stage_2_plotter::stage_2_plotter(stage_1_plotter &prior_stage) :
        telem_staged_plotter<stage_1_plotter>(prior_stage),
        processed_data(prior_stage.get_processed_data()) {
}

const telem_data &stage_2_plotter::get_processed_data() const {
    return processed_data;
}

void stage_2_plotter::plotter_draw(mglGraph *gr) {
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

void stage_2_plotter::plotter_calc() {
    data.Create(5, 1);

    prior_stage.join();
    const std::map<double, vector2d> &v_stage_1 = prior_stage.get_result();

    const std::map<double, double> &velocities = processed_data.get_velocities();
    const std::map<double, double> &altitudes = processed_data.get_altitudes();

    // Split data into signal vectors
    std::vector<double> times;
    std::vector<double> x_velocities;
    std::vector<double> y_velocities;

    for (const auto &item : v_stage_1) {
        double t = item.first;
        const vector2d &v = item.second;

        times.push_back(t);
        x_velocities.push_back(v.get_x());
        y_velocities.push_back(v.get_y());
    }

    // Process with LPF
    digital_filter lpf{PM_LPF_COEFFS};
    std::vector<double> x_velocities_filtered = lpf.transform(x_velocities);
    std::vector<double> y_velocities_filtered = lpf.transform(y_velocities);

    // FIR filters have constant delay
    unsigned int fir_delay = PM_LPF_COEFFS.size() / 2;

    // Update the result
    for (unsigned int i = fir_delay; i < x_velocities_filtered.size(); ++i) {
        double t = times[i];
        double vx = x_velocities_filtered[i];
        double vy = y_velocities_filtered[i];

        result[t] = {vx, vy};
    }

    // Plotting
    auto v_it = velocities.cbegin();
    auto alt_it = altitudes.cbegin();
    auto v_f_it = result.cbegin();

    double last_t = 0;
    double v_y_f_integral = 0;

    int time_steps = result.size();
    for (int i = 0; i < time_steps; ++i, ++v_it, ++alt_it, ++v_f_it) {
        double t = v_it->first;
        double v = v_it->second;
        double alt = alt_it->second;
        vector2d v_filtered = v_f_it->second;

        double dt = t - last_t;
        last_t = t;

        v_y_f_integral += v_filtered.get_y() * dt / 1000;

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
            data.Put(v_filtered.get_x(), 1, i);
            // 2: Velocity Y
            data.Put(v_filtered.get_y(), 2, i);

            // 3: Velocity Error
            data.Put(v_filtered.mag() - v, 3, i);
            // 4: Altitude Error
            data.Put(v_y_f_integral - alt, 4, i);
        }

        Check();
        plotter_update();
    }
}
