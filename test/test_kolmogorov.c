/* test_kolmogorov.c
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
#include "kolmogorov.h"
#include "test_common.h"

int test_kolmogorov() {
	ASSERT_ALMOST_EQUAL(plfit_kolmogorov(3.252691), 1.292e-09, 1e-11);
	ASSERT_ALMOST_EQUAL(plfit_kolmogorov(1.0), 0.27, 1e-3);
	ASSERT_ALMOST_EQUAL(plfit_kolmogorov(0.8), 0.5442, 1e-3);
	ASSERT_ALMOST_EQUAL(plfit_kolmogorov(0.4949748), 0.967, 1e-3);
	ASSERT_ALMOST_EQUAL(plfit_ks_test_one_sample_p(0.1149, 100), 0.1426, 1e-3);
	ASSERT_ALMOST_EQUAL(plfit_ks_test_one_sample_p(0.2553,  10), 0.5322, 1e-3);
	ASSERT_ALMOST_EQUAL(plfit_ks_test_one_sample_p(0.4435, 100), 2.2e-16, 1e-14);
	ASSERT_ALMOST_EQUAL(plfit_ks_test_two_sample_p(0.021, 1218, 1588), 0.917, 1e-2);
	ASSERT_ALMOST_EQUAL(plfit_ks_test_two_sample_p(0.07, 100, 100), 0.967, 1e-2);
	ASSERT_ALMOST_EQUAL(plfit_ks_test_two_sample_p(0.5, 30, 50), 9.065e-05, 1e-4);
	return 0;
}

int main(int argc, char* argv[]) {
	return test_kolmogorov();
}
