/**
 * @file
 */

#ifndef TELEM_FILTER_FILTER_H
#define TELEM_FILTER_FILTER_H

#include <vector>

/**
 * @brief Represents a filter consisting of the transfer
 * function coefficients.
 */
class filter {
private:
    /**
     * Filter numerator coefficients.
     */
    std::vector<double> b;
    /**
     * Filter denominator coefficients.
     */
    std::vector<double> a;

public:
    /**
     * Creates a new filter with the given coefficients.
     *
     * @param b the numerator coefficients
     * @param a the denominator coefficients
     */
    filter(std::vector<double> b,
           std::vector<double> a);

    /**
     * Creates a new FIR filter with the given filter
     * numerator coefficients and the denominator
     * coefficients set to a size 1 vector whose only
     * coefficient is 1.
     *
     * @param b the numerator coefficients
     */
    explicit filter(std::vector<double> b);

    /**
     * Performs a 1-D transformation of the given signal by
     * the filter transfer function.
     *
     * @param signal the signal to transform
     * @return the resulting signal
     */
    std::vector<double> transform(const std::vector<double> &signal);
};

#endif // TELEM_FILTER_FILTER_H
