#include "filter.h"

#include <deque>

filter::filter(std::vector<double> b,
               std::vector<double> a) :
        b(std::move(b)), a(std::move(a)) {
}

filter::filter(std::vector<double> b) :
        filter::filter(std::move(b), {1}) {
}

std::vector<double> filter::transform(const std::vector<double> &signal) {
    std::vector<double> result;
    result.reserve(signal.size());

    std::deque<double> x_buf;
    x_buf.resize(b.size(), 0);
    std::deque<double> y_buf;
    y_buf.resize(a.size(), 0);

    for (double x : signal) {
        x_buf.push_front(x);
        x_buf.resize(x_buf.size() - 1);

        double x_sigma = 0;
        for (int i = 0; i < x_buf.size(); ++i) {
            x_sigma += b[i] * x_buf[i];
        }

        double y_sigma = 0;
        for (int i = 1; i < y_buf.size(); ++i) {
            y_sigma += a[i] * y_buf[i];
        }

        double y = (x_sigma - y_sigma) / a[0];
        y_buf.push_front(y);
        y_buf.resize(y_buf.size() - 1);

        result.push_back(y);
    }

    return result;
}
