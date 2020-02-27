/* test_real.c
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

double data[41000];

int test_celegans() {
	plfit_result_t result;
	plfit_discrete_options_t options;
	size_t n;

	plfit_discrete_options_init(&options);
    options.p_value_method = PLFIT_P_VALUE_SKIP;

	n = test_read_file("celegans-indegree.dat", data, 41000);
	ASSERT_NONZERO(n);

	result.alpha = result.xmin = result.L = 0;
    plfit_discrete(data, n, &options, &result);
	ASSERT_ALMOST_EQUAL(result.alpha, 2.9967, 1e-1);
	ASSERT_EQUAL(result.xmin, 10);
	ASSERT_ALMOST_EQUAL(result.L, -245.14869, 1e-4);
	ASSERT_ALMOST_EQUAL(result.D, 0.04448, 1e-3);

	n = test_read_file("celegans-outdegree.dat", data, 41000);
	ASSERT_NONZERO(n);

	result.alpha = result.xmin = result.L = 0;
    plfit_discrete(data, n, &options, &result);
	ASSERT_ALMOST_EQUAL(result.alpha, 3.3778, 1e-1);
	ASSERT_EQUAL(result.xmin, 11);
	ASSERT_ALMOST_EQUAL(result.L, -232.80207, 1e-4);
	ASSERT_ALMOST_EQUAL(result.D, 0.08615, 1e-3);

	n = test_read_file("celegans-totaldegree.dat", data, 41000);
	ASSERT_NONZERO(n);

	result.alpha = result.xmin = result.L = 0;
    plfit_discrete(data, n, &options, &result);
	ASSERT_ALMOST_EQUAL(result.alpha, 3.29264, 1e-1);
	ASSERT_EQUAL(result.xmin, 18);
	ASSERT_ALMOST_EQUAL(result.L, -315.78214, 1e-4);
	ASSERT_ALMOST_EQUAL(result.D, 0.04760, 1e-3);

	return 0;
}

int test_condmat() {
	plfit_result_t result;
	plfit_discrete_options_t options;
	size_t n;

	plfit_discrete_options_init(&options);
    options.p_value_method = PLFIT_P_VALUE_SKIP;

	n = test_read_file("condmat2005-degree.dat", data, 41000);
	ASSERT_NONZERO(n);

	result.alpha = result.xmin = result.L = 0;
    plfit_discrete(data, n, &options, &result);
	ASSERT_ALMOST_EQUAL(result.alpha, 3.68612, 1e-2);
	ASSERT_EQUAL(result.xmin, 49);
	ASSERT_ALMOST_EQUAL(result.L, -3152.48302, 1e-4);
	ASSERT_ALMOST_EQUAL(result.D, 0.02393, 1e-3);

	return 0;
}

int main(int argc, char* argv[]) {
	int retval;

	retval = test_celegans();
	if (retval)
		return retval;

	retval = test_condmat();
	if (retval)
		return retval;

	return 0;
}
