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

/********** Continuous power law distribution fitting **********/

int plfit_estimate_alpha_continuous(double* xs, int n, double xmin, double* alpha);
int plfit_estimate_alpha_continuous_sorted(double* xs, int n, double xmin, double* alpha);
int plfit_log_likelihood_continuous(double* xs, int n, double alpha, double xmin, double* L);
int plfit_continuous(double* xs, int n, double* alpha, double* xmin);

/********** Discrete power law distribution fitting **********/

int plfit_estimate_alpha_discrete(double* xs, int n, double xmin, double* alpha);
int plfit_estimate_alpha_discrete_in_range(double* xs, int n, double xmin,
		double alpha_min, double alpha_max, double alpha_step, double* alpha);
int plfit_estimate_alpha_discrete_fast(double* xs, int n, double xmin, double* alpha);
int plfit_log_likelihood_discrete(double* xs, int n, double alpha, double xmin, double* L);
int plfit_discrete(double* xs, int n, double* alpha, double* xmin);
int plfit_discrete_in_range(double* xs, int n, double alpha_min, double alpha_max,
		double alpha_step, double* alpha, double* xmin);

__END_DECLS

#endif /* __ZETA_H__ */

