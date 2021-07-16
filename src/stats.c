/* stats.c
 *
 * Copyright (C) 2012 Tamas Nepusz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <math.h>
#include <stdlib.h>
#include "plfit_error.h"

int plfit_moments(double* xs, size_t n, double* mean, double* variance,
        double* skewness, double* kurtosis) {
    double mean0, variance0, skewness0, kurtosis0, error;
    double x, y;
    double *p, *end;

    /* Handle the trivial case of n = 0 */
    if (n == 0) {
        if (mean != 0) {
            *mean = 0.0;
        }
        if (variance != 0) {
            *variance = 0.0;
        }
        if (skewness != 0) {
            *skewness = 0.0;
        }
        if (kurtosis != 0) {
            *kurtosis = 0.0;
        }
        return PLFIT_SUCCESS;
    }

    /* Handle the trivial case of n = 1 */
    if (n == 1) {
        if (mean != 0) {
            *mean = xs[0];
        }
        if (variance != 0) {
            *variance = 0.0;
        }
        if (skewness != 0) {
            *skewness = 0.0;
        }
        if (kurtosis != 0) {
            *kurtosis = 0.0;
        }
        return PLFIT_SUCCESS;
    }

    end = xs + n;

    /* First pass: calculate the mean */
    mean0 = 0;
    for (p = xs; p != end; p++) {
        mean0 += *p;
    }
    mean0 /= n;
    if (mean != 0) {
        *mean = mean0;
    }

    /* If we don't need variance, skewness or kurtosis, we can exit here */
    if (!variance && !skewness && !kurtosis)
        return PLFIT_SUCCESS;

    /* Second pass: get the moments */
    error = variance0 = skewness0 = kurtosis0 = 0;
    for (p = xs; p != end; p++) {
        x = *p - mean0;
        error += x;

        y = x*x;
        variance0 += y;

        y *= x;
        skewness0 += y;

        y *= x;
        kurtosis0 += y;
    }
    variance0 -= error*error/n;

    if (variance != 0) {
        *variance = variance0/(n-1);
    }

    variance0 /= n;
    if (skewness != 0) {
        *skewness = skewness0 / (n*variance0*sqrt(variance0));
    }
    if (kurtosis != 0) {
        *kurtosis = kurtosis0 / (n*variance0*variance0);
    }

    return PLFIT_SUCCESS;
}
