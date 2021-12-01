/* test_underflow_handling.c
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

int test_underflow_handling() {
	plfit_result_t result;
	plfit_discrete_options_t options;
	double data[10000];
	size_t n;

	plfit_discrete_options_init(&options);
    options.p_value_method = PLFIT_P_VALUE_SKIP;

	n = test_read_file("underflow_regression_test_1.txt", data, 10000);
	ASSERT_NONZERO(n);

	/* I am not testing the expected outcome here because I don't know
	 * for sure what the output should be. All that we are about is that
	 * the calculation should not fail with an underflow error */
	if (plfit_discrete(data, n, &options, &result)) {
		return 1;
	}

	n = test_read_file("underflow_regression_test_2.txt", data, 10000);
	ASSERT_NONZERO(n);

	/* I am not testing the expected outcome here because I don't know
	 * for sure what the output should be. All that we are about is that
	 * the calculation should not fail with an underflow error */
	if (plfit_log_likelihood_discrete(data, n, result.alpha, result.xmin, &result.L)) {
		return 1;
	}

	return 0;
}

int main(int argc, char* argv[]) {
	return test_underflow_handling();
}
