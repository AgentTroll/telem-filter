#include "vector2d.h"

#include <cmath>

vector2d::vector2d(double x, double y) :
        x(x), y(y) {
}

vector2d::vector2d(double xy) :
        vector2d::vector2d(xy, xy) {
}

vector2d::vector2d()
        : vector2d::vector2d(0) {
}

double vector2d::get_x() const {
    return x;
}

double vector2d::get_y() const {
    return y;
}

double vector2d::mag() const {
    return std::sqrt(x * x + y * y);
}
