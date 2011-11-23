/* test_discrete.c
 *
 * Copyright (C) 2011 Tamas Nepusz
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

#include <math.h>
#include "plfit.h"
#include "test_common.h"

int test_discrete() {
	plfit_result_t result;
	double data[10000];
	size_t n;

	n = test_read_file("discrete_data.txt", data, 10000);
	ASSERT_NONZERO(n);

	result.alpha = 2.58;
	result.xmin = 2;
	plfit_log_likelihood_discrete(data, n, result.alpha, result.xmin, &result.L);
	ASSERT_ALMOST_EQUAL(result.L, -9155.62809, 1e-4);

	result.alpha = 0;
	result.xmin = 2;
	plfit_estimate_alpha_discrete_fast(data, n, result.xmin, 0, &result);
	ASSERT_ALMOST_EQUAL(result.alpha, 2.43, 1e-1);

	result.alpha = 0;
	result.xmin = 2;
	plfit_estimate_alpha_discrete(data, n, result.xmin, 0, &result);
	ASSERT_ALMOST_EQUAL(result.alpha, 2.58, 1e-1);

	result.alpha = result.xmin = result.L = 0;
	plfit_discrete_in_range(data, n, 1.01, 5.0, 0.01, 0, &result);
	ASSERT_ALMOST_EQUAL(result.alpha, 2.58, 1e-1);
	ASSERT_EQUAL(result.xmin, 2);
	ASSERT_ALMOST_EQUAL(result.L, -9155.62809, 1e-4);

	result.alpha = result.xmin = result.L = 0;
    plfit_discrete(data, n, 0, &result);
	ASSERT_ALMOST_EQUAL(result.alpha, 2.58035, 1e-1);
	ASSERT_EQUAL(result.xmin, 2);
	ASSERT_ALMOST_EQUAL(result.L, -9155.617067, 1e-4);

	return 0;
}

int main(int argc, char* argv[]) {
	return test_discrete();
}

