/* test_hsl_sf_lnhzeta.c
 *
 * Copyright (C) 2022 Tamas Nepusz
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
#include "hzeta.h"
#include "test_common.h"

int test_hsl_sf_lnhzeta() {
    double alpha = 2.58;
    double xmin = 2;

    ASSERT_ALMOST_EQUAL(hsl_sf_lnhzeta(alpha, xmin), -1.16389915, 1e-6);

	return 0;
}

int main(int argc, char* argv[]) {
	return test_hsl_sf_lnhzeta();
}
