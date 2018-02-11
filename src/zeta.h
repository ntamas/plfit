/* This file was initially taken from the GNU Scientific Library (GSL).
 * Some material were afterwards added from a private scientific library
 * based on GSL coined Home Scientific Libray (HSL) by its author
 * Jerome Benoit; this very material is itself inspired from the intial
 * material written by G. Jungan and distributed by GSL.
 * Ultimately, some modifications were done in order to render the
 * imported material independent from the rest of GSL.
*/

/* Part imported from GSL */
/* specfunc/gsl_sf_zeta.h
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2004 Gerard Jungman
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

/* Author:  G. Jungman */

/* Part imported from HSL */
/* `hsl/specfunc/zeta_nderiv.c' C source file
// HSL - Home Scientific Library
// Copyright (C) 2017-2018  Jerome Benoit
//
// HSL is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/* Author:  Jerome G. Benoit < jgmbenoit _at_ rezozer _dot_ net > */


#ifndef __ZETA_H__
#define __ZETA_H__

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


/* Part imported from GSL */
/* Hurwitz Zeta Function
 * zeta(s,q) = Sum[ (k+q)^(-s), {k,0,Infinity} ]
 *
 * s > 1.0, q > 0.0
 */
double gsl_sf_hzeta(const double s, const double q);
/* End of Part imported from GSL */

/* Part imported from HSL */
/* First Derivative of Hurwitz Zeta Function
 * zeta'(s,q) = - Sum[ Ln(k+q)/(k+q)^(s), {k,0,Infinity} ]
 *
 * s > 1.0, q > 0.0
 */
double hsl_sf_hzeta_deriv(const double s, const double q);

/* Second Derivative of Hurwitz Zeta Function
 * zeta''(s,q) = + Sum[ Ln(k+q)^2/(k+q)^(s), {k,0,Infinity} ]
 *
 * s > 1.0, q > 0.0
 */
double hsl_sf_hzeta_deriv2(const double s, const double q);
/* End of Part imported from HSL */

__END_DECLS

#endif /* __ZETA_H__ */

