/* test.c
 *
 * Copyright (C) 2010 Tamas Nepusz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
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

#include <stdio.h>
#include <math.h>
#include "kolmogorov.h"
#include "plfit.h"

#define RUN_TEST_CASE(func) do { \
	int result; \
	result = func(); \
	if (result != 0) return result; \
} while (0)
#define ASSERT_ALMOST_EQUAL(obs, exp, eps) do { \
	double diff = (obs) - (exp); \
	if (isnan(diff) || diff < -eps || diff > eps) { \
		fprintf(stderr, "%s:%d : expected %.8f, got %.8f\n", \
				__FILE__, __LINE__, (double)exp, (double)obs); \
		return 1; \
	} \
} while (0)
#define ASSERT_EQUAL(obs, exp) do { \
	if (obs != exp) { \
		fprintf(stderr, "%s:%d : expected %.8f, got %.8f\n", \
				__FILE__, __LINE__, (double)exp, (double)obs); \
		return 1; \
	} \
} while (0)

size_t test_read_file(const char* fname, double* data, int max_n) {
	FILE* f = fopen(fname, "r");
	size_t n = 0;

	if (!f)
		return 0;

	while (!feof(f) && n < max_n) {
		if (fscanf(f, "%lf", data+n)) {
			n++;
		}
	}

	fclose(f);
	return n;
}

int test_kolmogorov() {
	ASSERT_ALMOST_EQUAL(plfit_kolmogorov(1.0), 0.27, 1e-3);
	ASSERT_ALMOST_EQUAL(plfit_kolmogorov(0.8), 0.5442, 1e-3);
	ASSERT_ALMOST_EQUAL(plfit_ks_test_one_sample_p(0.2553, 10), 0.5322, 1e-3);
	ASSERT_ALMOST_EQUAL(plfit_ks_test_two_sample_p(0.021, 1218, 1588), 0.917, 1e-2);
	ASSERT_ALMOST_EQUAL(plfit_ks_test_two_sample_p(0.5, 30, 50), 9.065e-05, 1e-4);
	return 0;
}

int test_continuous() {
	plfit_result_t result;
	double data[10000];
	size_t n;

	n = test_read_file("../data/continuous_data.txt", data, 10000);

	result.alpha = 2.53282;
	result.xmin = 1.43628;
	plfit_log_likelihood_continuous(data, n, result.alpha, result.xmin, &result.L);
	ASSERT_ALMOST_EQUAL(result.L, -9276.42, 1e-1);

	result.alpha = 0;
	result.xmin = 1.43628;
	plfit_estimate_alpha_continuous(data, n, result.xmin, &result.alpha);
	ASSERT_ALMOST_EQUAL(result.alpha, 2.53282, 1e-4);

	result.alpha = result.xmin = result.L = 0;
	plfit_continuous(data, n, &result);
	ASSERT_ALMOST_EQUAL(result.alpha, 2.53282, 1e-4);
	ASSERT_ALMOST_EQUAL(result.xmin, 1.43628, 1e-4);
	ASSERT_ALMOST_EQUAL(result.L, -9276.42, 1e-1);

	return 0;
}

int test_discrete() {
	plfit_result_t result;
	double data[10000];
	size_t n;

	n = test_read_file("../data/discrete_data.txt", data, 10000);

	result.alpha = 2.58;
	result.xmin = 2;
	plfit_log_likelihood_discrete(data, n, result.alpha, result.xmin, &result.L);
	ASSERT_ALMOST_EQUAL(result.L, -9155.62809, 1e-4);

	result.alpha = 0;
	result.xmin = 2;
	plfit_estimate_alpha_discrete_fast(data, n, result.xmin, &result.alpha);
	ASSERT_ALMOST_EQUAL(result.alpha, 2.43, 1e-1);

	result.alpha = 0;
	result.xmin = 2;
	plfit_estimate_alpha_discrete(data, n, result.xmin, &result.alpha);
	ASSERT_ALMOST_EQUAL(result.alpha, 2.58, 1e-1);

	result.alpha = result.xmin = result.L = 0;
	plfit_discrete(data, n, &result);
	ASSERT_ALMOST_EQUAL(result.alpha, 2.58, 1e-1);
	ASSERT_EQUAL(result.xmin, 2);
	ASSERT_ALMOST_EQUAL(result.L, -9155.62809, 1e-4);

	return 0;
}

int test() {
	RUN_TEST_CASE(test_kolmogorov);
	RUN_TEST_CASE(test_continuous);
	RUN_TEST_CASE(test_discrete);

	return 0;
}
