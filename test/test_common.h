/* test_common.h
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

#include <stdio.h>
#include <stdlib.h>

#define RUN_TEST_CASE(func, label) do { \
	int result; \
	fprintf(stderr, "Testing " label "...\n"); \
	result = func(); \
	if (result != 0) return result; \
} while (0)
#define ASSERT_ALMOST_EQUAL(obs, exp, eps) do { \
	double diff = (obs) - (exp); \
	if (isnan(diff) || diff < -eps || diff > eps) { \
		fprintf(stderr, "%s:%d : expected %.8f, got %.8f, difference: %g\n", \
				__FILE__, __LINE__, (double)exp, (double)obs, diff); \
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
#define ASSERT_NONZERO(obs) do { \
	if (!obs) { \
		fprintf(stderr, "%s:%d : expected nonzero value, got %.8f\n", \
				__FILE__, __LINE__, (double)obs); \
		return 1; \
	} \
} while (0)

size_t test_read_file(const char* fname, double* data, int max_n);
