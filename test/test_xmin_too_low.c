/* test_xmin_too_low.c
 *
 * Copyright (C) 2021 Tamas Nepusz
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

int test_xmin_too_low() {
	plfit_result_t result;
	plfit_discrete_options_t options;
	double data[] = { 8, 8, 3, 2, 0, 0, 0, 0 };

	plfit_discrete_options_init(&options);
    options.p_value_method = PLFIT_P_VALUE_SKIP;

	if (plfit_discrete(data, sizeof(data) / sizeof(data[0]), &options, &result)) {
		return 1;
	}

    if (result.xmin < 2) {
        return 2;
    }

	return 0;
}

int test_xmin_too_low_and_not_enough_discrete_values_1() {
	plfit_result_t result;
	plfit_discrete_options_t options;
	double data[] = { 8, 8, 3, 3, 0, 0 };

	plfit_discrete_options_init(&options);
    options.p_value_method = PLFIT_P_VALUE_SKIP;

	if (plfit_discrete(data, sizeof(data) / sizeof(data[0]), &options, &result)) {
		return 1;
	}

    if (result.xmin < 3) {
        return 2;
    }

	return 0;
}

int test_xmin_too_low_and_not_enough_discrete_values_2() {
	plfit_result_t result;
	plfit_discrete_options_t options;
	double data[] = { 8, 8, 0, 0, 0, 0 };

	plfit_discrete_options_init(&options);
    options.p_value_method = PLFIT_P_VALUE_SKIP;

	if (plfit_discrete(data, sizeof(data) / sizeof(data[0]), &options, &result)) {
		return 1;
	}

    if (result.xmin < 8) {
        return 2;
    }

	return 0;
}

int main(int argc, char* argv[]) {
	RUN_TEST_CASE(
        test_xmin_too_low,
        "smallest x less than 1 but there are enough distinct values "
        "in the remainder after skipping them"
    );
    RUN_TEST_CASE(
        test_xmin_too_low_and_not_enough_discrete_values_1,
        "smallest x less than 1 and there are not enough distinct values "
        "in the remainder after skipping them (1)"
    );
    RUN_TEST_CASE(
        test_xmin_too_low_and_not_enough_discrete_values_2,
        "smallest x less than 1 and there are not enough distinct values "
        "in the remainder after skipping them (2)"
    );
}
