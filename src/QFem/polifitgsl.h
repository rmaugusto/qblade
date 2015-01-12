#ifndef POLIFITGSL_H
#define POLIFITGSL_H

#include <gsl/gsl_multifit.h>
#include <stdbool.h>
#include <math.h>
#include <vector>
/**
    This function is sourced from
    http://rosettacode.org/wiki/Polynomial_regression#C
    The code was adjusted to not use the most peverse arrangement
    of points and references ever encoutered in c++.
    @param obs - Length of data
    @param degree - degree of polynomial fit- Degree 7 gives ....x^6
    @param dx x data
    @param dy y data
    @param store output coefficients.
*/
bool polynomialfit(int obs, int degree,
                   std::vector<double> dx,
                   std::vector<double> dy,
                   std::vector<double> &store); /* n, p */

#endif // POLIFITGSL_H
