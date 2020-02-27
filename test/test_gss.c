/* test_gss.c
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
#include "gss.h"
#include "test_common.h"

double x_squared(void *instance, double x) {
    return x*x;
}

double minus_exp_minus_x_times_sin_x(void *instance, double x) {
    return -exp(-x) * sin(x);
}

int progress_report(void *instance, double x, double fx, double min, double fmin,
        double a, double b, int k) {
    printf("Iteration #%d: [%.4f; %.4f), x=%.4f, fx=%.4f, min=%.4f, fmin=%.4f\n",
            k, a, b, x, fx, min, fmin);
    return 0;
}

int test_x_squared() {
    double min = 42, fmin = 42;
    gss_parameter_t params;

    /* Default parameters, progress function given */
    min = fmin = 42;
    ASSERT_SUCCESSFUL(gss(-1, 1, &min, &fmin, x_squared, progress_report, 0, 0));
    ASSERT_ALMOST_EQUAL(min, 0, 1e-10);
    ASSERT_ALMOST_EQUAL(fmin, 0, 1e-10);

    /* Custom parameters, progress function given */
    gss_parameter_init(&params);
    params.epsilon = 1e-5;
    min = fmin = 42;
    ASSERT_SUCCESSFUL(gss(-1, 1, &min, &fmin, x_squared, progress_report, 0, &params));
    ASSERT_ALMOST_EQUAL(min, 0, 1e-5);
    ASSERT_ALMOST_EQUAL(fmin, 0, 1e-5);

    /* Custom parameters, progress function given */
    gss_parameter_init(&params);
    params.epsilon = 1e-5;
    min = fmin = 42;
    ASSERT_SUCCESSFUL(gss(-1, 1, &min, &fmin, x_squared, progress_report, 0, &params));
    ASSERT_ALMOST_EQUAL(min, 0, 1e-5);
    ASSERT_ALMOST_EQUAL(fmin, 0, 1e-5);

    /* Custom parameters, no progress function */
    min = fmin = 42;
    ASSERT_SUCCESSFUL(gss(-1, 1, &min, &fmin, x_squared, 0, 0, &params));
    ASSERT_ALMOST_EQUAL(min, 0, 1e-5);
    ASSERT_ALMOST_EQUAL(fmin, 0, 1e-5);

    /* Custom parameters, no progress function, no fmin */
    min = fmin = 42;
    ASSERT_SUCCESSFUL(gss(-1, 1, &min, 0, x_squared, 0, 0, &params));
    ASSERT_ALMOST_EQUAL(min, 0, 1e-5);
    ASSERT_EQUAL(fmin, 42);

    /* Custom parameters, no progress function, no min */
    min = fmin = 42;
    ASSERT_SUCCESSFUL(gss(-1, 1, 0, &fmin, x_squared, 0, 0, &params));
    ASSERT_EQUAL(min, 42);
    ASSERT_ALMOST_EQUAL(fmin, 0, 1e-5);

    /* Custom parameters, no progress function, no min, no fmin */
    min = fmin = 42;
    ASSERT_SUCCESSFUL(gss(-1, 1, 0, 0, x_squared, 0, 0, &params));
    ASSERT_EQUAL(min, 42);
    ASSERT_EQUAL(fmin, 42);

    return 0;
}

int test_minus_exp_minus_x_times_sin_x() {
    double min, fmin;
    gss_parameter_t param;

    gss_parameter_init(&param);
    param.epsilon = 1e-5;
    ASSERT_SUCCESSFUL(gss(0, 1.5, &min, &fmin, minus_exp_minus_x_times_sin_x,
                progress_report, 0, &param));
    ASSERT_ALMOST_EQUAL(min, 0.785396, 1e-5);
    ASSERT_ALMOST_EQUAL(fmin, -0.322397, 1e-5);

    return 0;
}

int main(int argc, char* argv[]) {
	RUN_TEST_CASE(test_x_squared, "GSS on f(x) = x^2");
	RUN_TEST_CASE(test_minus_exp_minus_x_times_sin_x,
            "GSS on f(x) = -e^x * sin(x)");

    return 0;
}
