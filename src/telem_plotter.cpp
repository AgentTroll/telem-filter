#include "telem_plotter.h"
#include "filter.h"

#include <FL/Fl.H>

/**
 * Parks-McClellan FIR coefficients:
 *   - 0-1 Hz break frequencies
 *   - 0.001 ripple deviation
 *   - LPF [1 0] coefficients
 *   - Sampled at ~30 Hz
 *   
 * Filter coefficients generated with MATLAB
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

telem_plotter::telem_plotter(const telem_plotter_data &plotter_data) :
        plotter_data(plotter_data) {
}

void telem_plotter::set_wnd(mglWnd *injected_wnd) {
    wnd = injected_wnd;
}

int telem_plotter::Draw(mglGraph *gr) {
    std::scoped_lock<std::mutex> lock{data_mutex};

    gr->Clf();

    gr->SubPlot(2, 2, 0);
    gr->Title("STEP 1: Time vs. X/Y Velocity");
    gr->Label('x', "Time (s)");
    gr->Label('y', "Velocity (m/s)");
    gr->Grid();
    gr->Box();
    gr->ClearLegend();
    gr->AddLegend("X", "r-");
    gr->AddLegend("Y", "b-");
    gr->Legend();
    gr->SetRanges(data.SubData(0), data.SubData(1));
    gr->Axis("xy");
    gr->Plot(data.SubData(0), data.SubData(3), "r-");
    gr->Plot(data.SubData(0), data.SubData(4), "b-");

    gr->SubPlot(2, 2, 1);
    gr->Title("STEP 3: Time vs. X/Y Velocity");
    gr->Label('x', "Time (s)");
    gr->Label('y', "Velocity (m/s)");
    gr->Grid();
    gr->Box();
    gr->ClearLegend();
    gr->AddLegend("X", "r-");
    gr->AddLegend("Y", "b-");
    gr->Legend();
    gr->SetRanges(data.SubData(0), data.SubData(1));
    gr->Axis("xy");
    gr->Plot(data.SubData(0), data.SubData(6), "r-");
    gr->Plot(data.SubData(0), data.SubData(7), "b-");

    gr->SubPlot(2, 2, 2);
    gr->Title("STEP 3: Altitude Error");
    gr->Label('x', "Time (s)");
    gr->Label('y', "Altitude Error (km)");
    gr->Grid();
    gr->Box();
    gr->SetRanges(data.SubData(0), data.SubData(8));
    gr->Axis("xy");
    gr->Plot(data.SubData(0), data.SubData(8));

    gr->SubPlot(2, 2, 3);
    gr->Title("STEP 3: Velocity Error");
    gr->Label('x', "Time (s)");
    gr->Label('y', "Velocity Error (m/s)");
    gr->Grid();
    gr->Box();
    gr->SetRanges(data.SubData(0), data.SubData(9));
    gr->Axis("xy");
    gr->Plot(data.SubData(0), data.SubData(9), "r-");

    return 0;
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

/**
 * Performs linear interpolation between each value in the
 * input map and writes the result into the output map.
 * 
 * This procedure is capable of in-place operation; i.e.
 * in is permitted to be the same as out.
 *
 * @param out the output map
 * @param in the input map
 */
static void lerp(std::map<double, double> &out, const std::map<double, double> &in) {
    std::vector<double> st;
    double last_unique_time = -1;
    double last_unique_value = -1;

    for (auto it = in.begin(); it != in.end(); it++) {
        double t = it->first;
        double v = it->second;
        if (v != last_unique_value || it == --in.end()) {
            out[t] = v;

            if (!st.empty()) {
                double slope = (v - last_unique_value) / (t - last_unique_time);
                for (const auto &interp_t : st) {
                    double dt = interp_t - last_unique_time;
                    out[interp_t] = last_unique_value + slope * dt;
                }
                st.clear();
            }

            last_unique_time = t;
            last_unique_value = v;
        } else {
            st.push_back(t);
        }
    }
}

/**
 * Adjusts the velocity vector to account for the rocket's
 * pitch maneuver.
 *
 * @param v the velocity magnitude, m/s
 * @param alt_prev the prior altitude, km
 * @param alt_next the altitude setpoint, km
 * @param v_x the X velocity reference, m/s
 * @param v_y the Y velocity reference, m/s
 * @param dt the time step, s
 */
static void adjust_vector(double v, double alt_prev, double alt_next,
                          double &v_x, double &v_y, double dt) {
    double dy = (alt_next - alt_prev) * 1000;
    v_y = std::copysign(std::min(dy / dt, v), dy);
    v_x = std::sqrt(v * v - v_y * v_y);
}

void telem_plotter::Calc() {
    // Initialize the processed data matrix
    data.Create(10,  // X = Data
                1); // Y = Time step

    // STEP 1:
    //   - Linear interpolation of altitudes
    //   - Derive initial guess for X/Y velocities
    telem_data &raw_data = plotter_data.raw_data;
    std::map<double, double> velocities = raw_data.get_velocities();
    std::map<double, double> altitudes = raw_data.get_altitudes();

    std::vector<double> times;
    std::vector<double> x_velocities_adjusted;
    std::vector<double> y_velocities_adjusted;

    lerp(altitudes, raw_data.get_altitudes());

    auto v_it = velocities.cbegin();
    auto alt_it = altitudes.cbegin();

    double last_t = 0;
    double v_y_a_integral = 0;

    int time_steps = velocities.size();
    for (int i = 0; i < time_steps; ++i, ++v_it, ++alt_it) {
        double t = v_it->first;
        double v = v_it->second;
        double alt = alt_it->second;

        double dt = t - last_t;
        last_t = t;

        double v_x_adjusted = 0;
        double v_y_adjusted = 0;
        adjust_vector(v, v_y_a_integral, alt, v_x_adjusted, v_y_adjusted, dt);
        v_y_a_integral += v_y_adjusted * dt / 1000;

        // Record data to the matrix
        {
            std::scoped_lock<std::mutex> lock{data_mutex};

            // Expand the data matrix for new data
            if (i != 0) {
                data.Insert('y', i);
            }

            // 0: Time
            data.Put(t, 0, i);
            // 1 - Velocity magnitude
            data.Put(v, 1, i);
            // 2: Altitude
            data.Put(alt, 2, i);

            // 3: Velocity X
            data.Put(v_x_adjusted, 3, i);
            // 4: Velocity Y
            data.Put(v_y_adjusted, 4, i);

            // 5: Altitude Error
            data.Put(v_y_a_integral - alt, 5, i);
        }

        // Record to output file
        csv_writer &v_writer = plotter_data.data_1_writer;
        v_writer << t << "," << v_x_adjusted << "," << v_y_adjusted << "," << alt << "\n";

        // C++ DSP signal vectors
        times.push_back(t);
        x_velocities_adjusted.push_back(v_x_adjusted);
        y_velocities_adjusted.push_back(v_y_adjusted);

        Check();

        Fl::awake(update_wnd, wnd);

        if (t > 200) {
            break;
        }
    }

    // STEP 2:
    //   - LP filter velocity
    filter lpf{PM_LPF_COEFFS};
    std::vector<double> x_velocities_raw_filtered = lpf.transform(x_velocities_adjusted);
    std::vector<double> y_velocities_raw_filtered = lpf.transform(y_velocities_adjusted);

    int fir_delay = PM_LPF_COEFFS.size() / 2;

    std::map<double, double> x_velocities_filtered;
    for (int i = fir_delay - 1; i < x_velocities_raw_filtered.size(); ++i) {
        double t = times[i];
        x_velocities_filtered[t] = x_velocities_raw_filtered[i];
    }

    std::map<double, double> y_velocities_filtered;
    for (int i = fir_delay - 1; i < y_velocities_raw_filtered.size(); ++i) {
        double t = times[i];
        y_velocities_filtered[t] = y_velocities_raw_filtered[i];
    }

    // STEP 3:
    //   - Re-adjust X velocity to offset velocity error
    auto v_x_f_it = x_velocities_filtered.cbegin();
    auto v_y_f_it = y_velocities_filtered.begin();
    v_y_f_it = y_velocities_filtered.begin();
    v_it = velocities.begin();
    alt_it = altitudes.cbegin();

    last_t = 0;
    double v_y_f_integral = 0;

    time_steps = x_velocities_filtered.size();
    for (int i = 0; i < time_steps; ++i, ++v_x_f_it, ++v_y_f_it, ++v_it, ++alt_it) {
        double t = v_x_f_it->first;
        double v = v_it->second;
        double v_y_f = v_y_f_it->second;
        double v_x_f = std::sqrt(v * v - std::min(v_y_f * v_y_f, v * v));
        double alt = alt_it->second;

        double dt = t - last_t;
        last_t = t;

        v_y_f_integral += v_y_f * dt / 1000;

        // Record data to the matrix
        {
            std::scoped_lock<std::mutex> lock{data_mutex};

            // 3: Velocity X
            data.Put(v_x_f, 6, i);
            // 4: Velocity Y
            data.Put(v_y_f, 7, i);

            // 5: Altitude Error
            data.Put(v_y_f_integral - alt, 8, i);
            // 5: Velocity Error
            data.Put(std::sqrt(v_x_f * v_x_f + v_y_f * v_y_f) - v, 9, i);
        }

        // Record to output file
        csv_writer &v_writer = plotter_data.data_3_writer;
        v_writer << t << "," << v_x_f << "," << v_y_f << "\n";

        Check();

        Fl::awake(update_wnd, wnd);
    }
}
