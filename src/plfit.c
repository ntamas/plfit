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

#include <stdio.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "error.h"
#include "lbfgs.h"
#include "platform.h"
#include "plfit.h"
#include "kolmogorov.h"
#include "zeta.h"

static int double_comparator(const void *a, const void *b) {
	const double *da = (const double*)a;
	const double *db = (const double*)b;
	return (*da > *db) - (*da < *db);
}

/********** Continuous power law distribution fitting **********/

void plfit_i_logsum_less_than_continuous(double* begin, double* end,
        double xmin, double* result, size_t* m) {
    double logsum = 0.0;
    size_t count = 0;

	for (; begin != end; begin++) {
		if (*begin >= xmin) {
			count++;
			logsum += log(*begin / xmin);
		}
	}

    *m = count;
    *result = logsum;
}

double plfit_i_logsum_continuous(double* begin, double* end, double xmin) {
    double logsum = 0.0;
	for (; begin != end; begin++)
        logsum += log(*begin / xmin);
    return logsum;
}

int plfit_estimate_alpha_continuous(double* xs, size_t n, double xmin, double* alpha) {
	double result;
	size_t m;

	if (xmin <= 0) {
		PLFIT_ERROR("xmin must be greater than zero", PLFIT_EINVAL);
	}

    plfit_i_logsum_less_than_continuous(xs, xs+n, xmin, &result, &m);

	if (m == 0) {
		PLFIT_ERROR("no data point was larger than xmin", PLFIT_EINVAL);
	}

	*alpha = 1 + m / result;

	return PLFIT_SUCCESS;
}

int plfit_estimate_alpha_continuous_sorted(double* xs, size_t n, double xmin, double* alpha) {
	double *end;

	if (xmin <= 0) {
		PLFIT_ERROR("xmin must be greater than zero", PLFIT_EINVAL);
	}

	end = xs + n;
	for (; xs != end && *xs < xmin; xs++);
	if (xs == end) {
		PLFIT_ERROR("no data point was larger than xmin", PLFIT_EINVAL);
	}

	*alpha = 1 + (end-xs) / plfit_i_logsum_continuous(xs, end, xmin);

	return PLFIT_SUCCESS;
}


int plfit_log_likelihood_continuous(double* xs, size_t n, double alpha, double xmin, double* L) {
	double logsum, c;
	size_t m;

	if (alpha <= 1) {
		PLFIT_ERROR("alpha must be greater than one", PLFIT_EINVAL);
	}
	if (xmin <= 0) {
		PLFIT_ERROR("xmin must be greater than zero", PLFIT_EINVAL);
	}

	c = (alpha - 1) / xmin;
    plfit_i_logsum_less_than_continuous(xs, xs+n, xmin, &logsum, &m);
	*L = -alpha * logsum + log(c) * m;

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

int plfit_continuous(double* xs, size_t n, unsigned short int finite_size_correction,
		plfit_result_t* result) {
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

	if (finite_size_correction) {
		best_alpha = best_alpha * (n-1) / n + 1.0 / n;
	}

	result->alpha = best_alpha;
	result->xmin  = best_xmin;
	result->D = best_D;
	result->p = plfit_ks_test_one_sample_p(best_D, n);
	plfit_log_likelihood_continuous(xs, n, result->alpha, result->xmin, &result->L);

	return PLFIT_SUCCESS;
}

/********** Discrete power law distribution fitting **********/

typedef struct {
    size_t m;
    double logsum;
    double xmin;
} plfit_i_estimate_alpha_discrete_data_t;

double plfit_i_logsum_discrete(double* begin, double* end, double xmin) {
	double logsum = 0.0;
	for (; begin != end; begin++)
		logsum += log(*begin);
    return logsum;
}

void plfit_i_logsum_less_than_discrete(double* begin, double* end, double xmin,
        double* logsum, size_t* m) {
	double result = 0.0;
	size_t count = 0;

	for (; begin != end; begin++) {
		if (*begin < xmin)
			continue;

		result += log(*begin);
		count++;
	}

    *logsum = result;
    *m = count;
}

lbfgsfloatval_t plfit_i_estimate_alpha_discrete_evaluate(
        void* instance, const lbfgsfloatval_t* x,
        lbfgsfloatval_t* g, const int n,
        const lbfgsfloatval_t step) {
    plfit_i_estimate_alpha_discrete_data_t* data;
    double dx = step;

    data = (plfit_i_estimate_alpha_discrete_data_t*)instance;

    /* Find the delta X value to estimate the gradient */
    if (dx > 0.001 || dx == 0)
        dx = 0.001;
    else if (dx < -0.001)
        dx = -0.001;

    g[0] = data->logsum + data->m *
        (log(gsl_sf_hzeta(x[0] + dx, data->xmin)) - log(gsl_sf_hzeta(x[0], data->xmin))) / dx;
    return x[0] * data->logsum + data->m * log(gsl_sf_hzeta(x[0], data->xmin));
}

int plfit_i_estimate_alpha_discrete_progress(void* instance,
        const lbfgsfloatval_t* x, const lbfgsfloatval_t* g,
        const lbfgsfloatval_t fx, const lbfgsfloatval_t xnorm,
        const lbfgsfloatval_t gnorm, const lbfgsfloatval_t step,
        int n, int k, int ls) {
    return 0;
}

int plfit_i_estimate_alpha_discrete(double* xs, size_t n, double xmin, double* alpha,
        plfit_bool_t sorted) {
    lbfgs_parameter_t param;
    lbfgsfloatval_t* variables;
    plfit_i_estimate_alpha_discrete_data_t data;
    int ret;

	if (xmin < 1) {
		PLFIT_ERROR("xmin must be at least 1", PLFIT_EINVAL);
	}

    /* Initialize algorithm parameters */
    lbfgs_parameter_init(&param);
    param.max_iterations = 0;   /* proceed until infinity */

    /* Set up context for optimization */
    data.xmin = xmin;
    if (sorted) {
        data.logsum = plfit_i_logsum_discrete(xs, xs+n, xmin);
        data.m = n;
    } else {
        plfit_i_logsum_less_than_discrete(xs, xs+n, xmin, &data.logsum, &data.m);
    }

    /* Allocate space for the single alpha variable */
    variables = lbfgs_malloc(1);
    variables[0] = 3.0;       /* initial guess */

    /* Optimization */
    ret = lbfgs(1, variables, /* ptr_fx = */ 0,
            plfit_i_estimate_alpha_discrete_evaluate,
            plfit_i_estimate_alpha_discrete_progress,
            &data, &param);

    if (ret < 0 &&
        ret != LBFGSERR_ROUNDING_ERROR &&
        ret != LBFGSERR_MAXIMUMLINESEARCH &&
        ret != LBFGSERR_CANCELED) {
        char buf[4096];
        snprintf(buf, 4096, "L-BFGS optimization signaled an error (error code = %d)", ret);
        lbfgs_free(variables);
        PLFIT_ERROR(buf, PLFIT_FAILURE);
    }
    *alpha = variables[0];
    
    /* Deallocate the variable array */
    lbfgs_free(variables);

    return PLFIT_SUCCESS;
}

int plfit_estimate_alpha_discrete(double* xs, size_t n, double xmin, double* alpha) {
    return plfit_i_estimate_alpha_discrete(xs, n, xmin, alpha, /* sorted = */ 0);
}

int plfit_estimate_alpha_discrete_fast(double* xs, size_t n, double xmin, double* alpha) {
	if (xmin < 1) {
		PLFIT_ERROR("xmin must be at least 1", PLFIT_EINVAL);
	}
	return plfit_estimate_alpha_continuous(xs, n, xmin-0.5, alpha);
}

int plfit_estimate_alpha_discrete_in_range(double* xs, size_t n, double xmin,
		double alpha_min, double alpha_max, double alpha_step, double *alpha) {
	double curr_alpha, best_alpha, L, L_max;
	double logsum;
	size_t m;

	if (xmin < 1) {
		PLFIT_ERROR("xmin must be at least 1", PLFIT_EINVAL);
	}
	if (alpha_min <= 1) {
		PLFIT_ERROR("alpha_min must be greater than 1", PLFIT_EINVAL);
	}
	if (alpha_max < alpha_min) {
		PLFIT_ERROR("alpha_max must be greater than alpha_min", PLFIT_EINVAL);
	}

    plfit_i_logsum_less_than_discrete(xs, xs+n, xmin, &logsum, &m);

	best_alpha = alpha_min; L_max = -DBL_MAX;
	for (curr_alpha = alpha_min; curr_alpha <= alpha_max; curr_alpha += alpha_step) {
		L = -curr_alpha * logsum - m * log(gsl_sf_hzeta(curr_alpha, xmin));
		if (L > L_max) {
			L_max = L;
			best_alpha = curr_alpha;
		}
	}

	*alpha = best_alpha;

	return PLFIT_SUCCESS;
}

int plfit_estimate_alpha_discrete_old(double* xs, size_t n, double xmin, double* alpha) {
	return plfit_estimate_alpha_discrete_in_range(xs, n, xmin, 1.5, 3.5, 0.01, alpha);
}

int plfit_log_likelihood_discrete(double* xs, size_t n, double alpha, double xmin, double* L) {
	double result;
	size_t m;

	if (alpha <= 1) {
		PLFIT_ERROR("alpha must be greater than one", PLFIT_EINVAL);
	}
	if (xmin <= 0) {
		PLFIT_ERROR("xmin must be greater than zero", PLFIT_EINVAL);
	}

    plfit_i_logsum_less_than_discrete(xs, xs+n, xmin, &result, &m);
	result = - alpha * result - m * log(gsl_sf_hzeta(alpha, xmin));

	*L = result;

	return PLFIT_SUCCESS;
}

int plfit_i_ks_test_discrete(double* xs, double* xs_end, const double alpha, const double xmin, double* D) {
	/* Assumption: xs is sorted and cut off at xmin so the first element is
	 * always larger than or equal to xmin. */
	double result = 0, n, hzeta, x;
	int m = 0;

	n = xs_end - xs;
	hzeta = gsl_sf_hzeta(alpha, xmin);

	while (xs < xs_end) {
		double d;

		x = *xs;
		d = fabs(1-(gsl_sf_hzeta(alpha, x) / hzeta) - m / n);

		if (d > result)
			result = d;

		do {
			xs++; m++;
		} while (xs < xs_end && *xs == x);
	}

	*D = result;

	return PLFIT_SUCCESS;
}

int plfit_discrete_in_range(double* xs, size_t n, double alpha_min, double alpha_max,
		double alpha_step, unsigned short int finite_size_correction,
		plfit_result_t* result) {
	double curr_D, curr_alpha;
    double best_D = DBL_MAX, best_xmin = 1, best_alpha = alpha_min;
	double *xs_copy, *px, *end, *end_xmin, prev_x;
	int m;

	if (n <= 0) {
		PLFIT_ERROR("no data points", PLFIT_EINVAL);
	}
	if (alpha_min <= 1.0) {
		PLFIT_ERROR("alpha_min must be greater than 1.0", PLFIT_EINVAL);
	}
	if (alpha_max < alpha_min) {
		PLFIT_ERROR("alpha_max must be greater than alpha_min", PLFIT_EINVAL);
	}

	/* Make a copy of xs and sort it */
	xs_copy = (double*)malloc(sizeof(double) * n);
	memcpy(xs_copy, xs, sizeof(double) * n);
	qsort(xs_copy, n, sizeof(double), double_comparator);

	best_D = DBL_MAX; best_xmin = 1; best_alpha = alpha_min;

	/* Make sure there are at least three distinct values if possible */
	px = xs_copy; end = px + n; end_xmin = end - 1; m = 0;
	prev_x = *end_xmin;
	while (*end_xmin == prev_x && end_xmin > px)
		end_xmin--;
	prev_x = *end_xmin;
	while (*end_xmin == prev_x && end_xmin > px)
		end_xmin--;

	prev_x = 0;
	while (px < end_xmin) {
		while (px < end_xmin && *px == prev_x) {
			px++; m++;
		}

		plfit_estimate_alpha_discrete_in_range(px, n-m, *px,
				alpha_min, alpha_max, alpha_step, &curr_alpha);
		plfit_i_ks_test_discrete(px, end, curr_alpha, *px, &curr_D);

		if (curr_D < best_D) {
			best_alpha = curr_alpha;
			best_xmin = *px;
			best_D = curr_D;
		}

		prev_x = *px;
		px++; m++;
	}

	free(xs_copy);

	if (finite_size_correction) {
		best_alpha = best_alpha * (n-1) / n + 1.0 / n;
	}

	result->alpha = best_alpha;
	result->xmin  = best_xmin;
	result->D = best_D;
	result->p = plfit_ks_test_one_sample_p(best_D, n);
	plfit_log_likelihood_discrete(xs, n, result->alpha, result->xmin, &result->L);

	return PLFIT_SUCCESS;
}

// TODO: this is an almost exact copy of plfit_discrete_in_range. Not really DRY.
int plfit_discrete(double* xs, size_t n, unsigned short int finite_size_correction,
		plfit_result_t* result) {
	double curr_D, curr_alpha;
    double best_D = DBL_MAX, best_xmin = 1, best_alpha = -1;
	double *xs_copy, *px, *end, *end_xmin, prev_x;
	int m;

	if (n <= 0) {
		PLFIT_ERROR("no data points", PLFIT_EINVAL);
	}

	/* Make a copy of xs and sort it */
	xs_copy = (double*)malloc(sizeof(double) * n);
	memcpy(xs_copy, xs, sizeof(double) * n);
	qsort(xs_copy, n, sizeof(double), double_comparator);

	/* Make sure there are at least three distinct values if possible */
	px = xs_copy; end = px + n; end_xmin = end - 1; m = 0;
	prev_x = *end_xmin;
	while (*end_xmin == prev_x && end_xmin > px)
		end_xmin--;
	prev_x = *end_xmin;
	while (*end_xmin == prev_x && end_xmin > px)
		end_xmin--;

	prev_x = 0;
	while (px < end_xmin) {
		while (px < end_xmin && *px == prev_x) {
			px++; m++;
		}

		plfit_i_estimate_alpha_discrete(px, n-m, *px, &curr_alpha, /* sorted = */ 1);
		plfit_i_ks_test_discrete(px, end, curr_alpha, *px, &curr_D);

		if (curr_D < best_D) {
			best_alpha = curr_alpha;
			best_xmin = *px;
			best_D = curr_D;
		}

		prev_x = *px;
		px++; m++;
	}

	free(xs_copy);

	if (finite_size_correction) {
		best_alpha = best_alpha * (n-1) / n + 1.0 / n;
	}

	result->alpha = best_alpha;
	result->xmin  = best_xmin;
	result->D = best_D;
	result->p = plfit_ks_test_one_sample_p(best_D, n);
	plfit_log_likelihood_discrete(xs, n, result->alpha, result->xmin, &result->L);

	return PLFIT_SUCCESS;
}

