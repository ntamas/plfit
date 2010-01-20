/* plfit.c
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

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"

int plfit_estimate_alpha_continuous(double* xs, int n, double xmin, double* alpha) {
	double *px, *end, result;
	int m;

	if (xmin <= 0) {
		PLFIT_ERROR("xmin must be greater than zero", PLFIT_EINVAL);
	}

	end = xs + n;
	for (px = xs, m = 0, result = 0; px != end; px++) {
		if (*px >= xmin) {
			m++;
			result += log(*px / xmin);
		}
	}

	if (m == 0) {
		PLFIT_ERROR("no data point was larger than xmin", PLFIT_EINVAL);
	}

	*alpha = 1 + m / result;

	return PLFIT_SUCCESS;
}

int plfit_estimate_alpha_continuous_sorted(double* xs, int n, double xmin, double* alpha) {
	double *px, *end, result;
	int m;

	if (xmin <= 0) {
		PLFIT_ERROR("xmin must be greater than zero", PLFIT_EINVAL);
	}

	end = xs + n;
	for (px = xs; px != end && *px < xmin; px++);
	for (m = 0, result = 0; px != end; px++) {
		m++;
		result += log(*px / xmin);
	}

	if (m == 0) {
		PLFIT_ERROR("no data point was larger than xmin", PLFIT_EINVAL);
	}

	*alpha = 1 + m / result;

	return PLFIT_SUCCESS;
}


int plfit_log_likelihood_continuous(double* xs, int n, double alpha, double xmin, double* L) {
	double *px, *end, result, c;
	int m;

	if (xmin <= 0) {
		PLFIT_ERROR("xmin must be greater than zero", PLFIT_EINVAL);
	}

	c = (alpha - 1) / xmin;
	end = xs + n;
	for (px = xs, m = 0, result = 0; px != end; px++) {
		if (*px < xmin)
			continue;

		result += log(*px / xmin);
		m++;
	}
	result = -alpha * result + log(c) * m;

	*L = result;

	return PLFIT_SUCCESS;
}

int plfit_i_ks_test_continuous(double* xs, double* xs_end, const double alpha, const double xmin, double* D) {
	/* Assumption: xs is sorted and cut off at xmin so the first element is
	 * always larger than or equal to xmin. */
	double result = 0, n;
	int m = 0;

	n = xs_end - xs;

	while (xs < xs_end) {
		double d = fabs(1-pow(xmin / *xs, alpha-1) - m / n);

		if (d > result)
			result = d;

		xs++; m++;
	}

	*D = result;

	return PLFIT_SUCCESS;
}

static int double_comparator(const void *a, const void *b) {
	const double *da = (const double*)a;
	const double *db = (const double*)b;
	return (*da > *db) - (*da < *db);
}

int plfit_continuous(double* xs, int n, double* alpha, double* xmin) {
	double curr_D, curr_alpha, best_D, best_xmin, best_alpha;
	double *xs_copy, *px, *end, prev_x;
	int m;

	if (n <= 0) {
		PLFIT_ERROR("no data points", PLFIT_EINVAL);
	}

	/* Make a copy of xs and sort it */
	xs_copy = (double*)malloc(sizeof(double) * n);
	memcpy(xs_copy, xs, sizeof(double) * n);
	qsort(xs_copy, n, sizeof(double), double_comparator);

	prev_x = 0; best_D = DBL_MAX; best_xmin = 0; best_alpha = 0;

	end = xs_copy + n - 1; px = xs_copy; m = 0;
	while (px < end) {
		while (px < end && *px == prev_x) {
			px++; m++;
		}

		plfit_estimate_alpha_continuous_sorted(px, n-m, *px, &curr_alpha);
		plfit_i_ks_test_continuous(px, end+1, curr_alpha, *px, &curr_D);

		if (curr_D < best_D) {
			best_alpha = curr_alpha;
			best_xmin = *px;
			best_D = curr_D;
		}

		prev_x = *px;
		px++; m++;
	}

	free(xs_copy);

	if (alpha)
		*alpha = best_alpha;
	if (xmin)
		*xmin = best_xmin;

	return PLFIT_SUCCESS;
}

