/**
 * @file
 */

#ifndef TELEM_FILTER_VECTOR2D_H
#define TELEM_FILTER_VECTOR2D_H

/**
 * @brief Represents a 2-D vector with double-precision
 * floating-point components.
 */
class vector2d {
private:
    /**
     * The X component of the vector.
     */
    double x;
    /**
     * The Y component of the vector.
     */
    double y;

public:
    /**
     * Creates a new vector with the given components.
     *
     * @param x the X component
     * @param y the Y component
     */
    vector2d(double x, double y);

    /**
     * Creates a new vector whose two components are equal
     * to the given value.
     *
     * @param xy the X and Y component value
     */
    explicit vector2d(double xy);

    /**
     * Creates a 0 vector, whose components are both set
     * equal to 0.
     */
    vector2d();

    /**
     * Determines the X component of this vector.
     *
     * @return the X component
     */
    [[nodiscard]] double get_x() const;

    /**
     * Determines the Y component of this vector.
     *
     * @return the Y component.
     */
    [[nodiscard]] double get_y() const;

    /**
     * Determines the magnitude of the vector, computed by
     * taking the 2-norm of the X and Y components.
     *
     * @return the magnitude of this vector
     */
    [[nodiscard]] double mag() const;
};

#endif // TELEM_FILTER_VECTOR2D_H
