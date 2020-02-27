/* test_continuous.c
 *
 * Copyright (C) 2011 Tamas Nepusz
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
#include "plfit.h"
#include "test_common.h"

int test_continuous() {
	plfit_result_t result;
	plfit_continuous_options_t options;
	double data[10000];
	size_t n;

	plfit_continuous_options_init(&options);
    options.p_value_method = PLFIT_P_VALUE_SKIP;

	n = test_read_file("continuous_data.txt", data, 10000);

	result.alpha = 2.53282;
	result.xmin = 1.43628;
	plfit_log_likelihood_continuous(data, n, result.alpha, result.xmin, &result.L);
	ASSERT_ALMOST_EQUAL(result.L, -9276.42, 1e-1);

	result.alpha = 0;
	result.xmin = 1.43628;
	plfit_estimate_alpha_continuous(data, n, result.xmin, &options, &result);
	ASSERT_ALMOST_EQUAL(result.alpha, 2.53282, 1e-4);

	result.alpha = result.xmin = result.L = 0;
	plfit_continuous(data, n, &options, &result);
	ASSERT_ALMOST_EQUAL(result.xmin, 1.43628, 1e-4);
	ASSERT_ALMOST_EQUAL(result.alpha, 2.53282, 1e-4);
	ASSERT_ALMOST_EQUAL(result.L, -9276.42, 1e-1);

	return 0;
}

int main(int argc, char* argv[]) {
	return test_continuous();
}
