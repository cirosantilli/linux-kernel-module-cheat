#ifndef LKMC_MATH_H
#define LKMC_MATH_H

#include <math.h>
#include <stdbool.h>

bool lkmc_vector_equal(size_t n, double *v1, double *v2, double max_err) {
    double sum = 0.0;
    double diff;
    size_t i;
    for (i = 0; i < n; ++i) {
        diff = v1[i] - v2[i];
        sum += diff * diff;
    }
    if (sqrt(sum)/n > max_err)
        return false;
    return true;
}

#endif
