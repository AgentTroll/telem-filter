#include "stage_1_plotter.h"

stage_1_plotter::stage_1_plotter(const telem_data_json &raw_data) :
        raw_data(raw_data) {
}

const telem_data &stage_1_plotter::get_processed_data() const {
    return processed_data;
}

void stage_1_plotter::plotter_draw(mglGraph *gr) {
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
 * @param v_mag the velocity magnitude, m/s
 * @param alt_prev the prior altitude, km
 * @param alt_next the altitude setpoint, km
 * @param dt the time step, s
 * @return the velocity components needed to reach the
 * prescribed altitude with the given magnitude
 */
static vector2d adjust_vector(double v_mag, double alt_prev, double alt_next, double dt) {
    double dy = (alt_next - alt_prev) * 1000;
    double v_y = std::copysign(std::min(dy / dt, v_mag), dy);
    double v_x = std::sqrt(v_mag * v_mag - v_y * v_y);

    return {v_x, v_y};
}

void stage_1_plotter::plotter_calc() {
    data.Create(5, 1);

    std::map<double, double> velocities = raw_data.get_velocities();
    std::map<double, double> altitudes = raw_data.get_altitudes();

    // Interpolate altitudes
    lerp(altitudes, raw_data.get_altitudes());

    // Record for use by the next stages
    processed_data = {velocities, altitudes};

    // Initial extraction + plotting loop
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

        // Extract velocity
        vector2d v_adjusted = adjust_vector(v, v_y_a_integral, alt, dt);
        result[t] = v_adjusted;

        v_y_a_integral += v_adjusted.get_y() * dt / 1000;

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

        if (t > 200) {
            break;
        }
    }
}
