/* plfit.h
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

#ifndef __PLFIT_H__
#define __PLFIT_H__

#include <stdlib.h>

#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif

__BEGIN_DECLS

#define PLFIT_VERSION_MAJOR 0
#define PLFIT_VERSION_MINOR 4
#define PLFIT_VERSION_STRING "0.4"

typedef unsigned short int plfit_bool_t;

typedef struct _plfit_result_t {
	double alpha;     /* fitted power-law exponent */
	double xmin;      /* cutoff where the power-law behaviour kicks in */
	double L;         /* log-likelihood of the sample */
	double D;         /* test statistic for the KS test */
	double p;         /* p-value of the KS test */
} plfit_result_t;

/********** continuous power law distribution fitting **********/

int plfit_log_likelihood_continuous(double* xs, size_t n, double alpha, double xmin, double* l);
int plfit_estimate_alpha_continuous(double* xs, size_t n, double xmin,
        plfit_bool_t finite_size_correction, plfit_result_t* result);
int plfit_estimate_alpha_continuous_sorted(double* xs, size_t n, double xmin,
        plfit_bool_t finite_size_correction, plfit_result_t* result);
int plfit_continuous(double* xs, size_t n, plfit_bool_t finite_size_correction,
		plfit_result_t* result);

/********** discrete power law distribution fitting **********/

int plfit_estimate_alpha_discrete(double* xs, size_t n, double xmin,
        plfit_bool_t finite_size_correction, plfit_result_t *result);
int plfit_estimate_alpha_discrete_fast(double* xs, size_t n, double xmin,
        plfit_bool_t finite_size_correction, plfit_result_t *result);
int plfit_estimate_alpha_discrete_old(double* xs, size_t n, double xmin,
        double alpha_min, double alpha_max, double alpha_step,
        plfit_bool_t finite_size_correction, plfit_result_t *result);
int plfit_log_likelihood_discrete(double* xs, size_t n, double alpha, double xmin, double* l);
int plfit_discrete(double* xs, size_t n, plfit_bool_t finite_size_correction,
		plfit_result_t* result);
int plfit_discrete_in_range(double* xs, size_t n, double alpha_min, double alpha_max,
		double alpha_step, plfit_bool_t finite_size_correction,
		plfit_result_t* result);

__END_DECLS

#endif /* __PLFIT_H__ */

