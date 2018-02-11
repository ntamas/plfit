/* vim:set ts=4 sw=2 sts=2 et: */

/* This file was initially taken from the GNU Scientific Library (GSL).
 * Some material were afterwards added from a private scientific library
 * based on GSL coined Home Scientific Libray (HSL) by its author
 * Jerome Benoit; this very material is itself inspired from the intial
 * material written by G. Jungan and distributed by GSL.
 * Ultimately, some modifications were done in order to render the
 * imported material independent from the rest of GSL.
*/

/* Part imported from GSL */
/* specfunc/zeta.c
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

/*
#include <config.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_sf_elementary.h>
#include <gsl/gsl_sf_exp.h>
#include <gsl/gsl_sf_gamma.h>
#include <gsl/gsl_sf_pow_int.h>
#include <gsl/gsl_sf_zeta.h>

#include "error.h"

#include "chebyshev.h"
#include "cheb_eval.c"
*/

#include <math.h>
#include <stdio.h>
#include "error.h"

/*-*-*-*-*-*-*-*-*-*- From gsl_machine.h -*-*-*-*-*-*-*-*-*-*-*-*-*/

#define GSL_LOG_DBL_MIN   (-7.0839641853226408e+02)
#define GSL_LOG_DBL_MAX    7.0978271289338397e+02
#define GSL_DBL_EPSILON        2.2204460492503131e-16

/*-*-*-*-*-*-*-*-*-* From gsl_sf_result.h *-*-*-*-*-*-*-*-*-*-*-*/

struct gsl_sf_result_struct {
  double val;
  double err;
};
typedef struct gsl_sf_result_struct gsl_sf_result;

/*-*-*-*-*-*-*-*-*-*-*-* Private Section *-*-*-*-*-*-*-*-*-*-*-*/

/* coefficients for Maclaurin summation in hzeta()
 * B_{2j}/(2j)!
 */
static double hzeta_c[15] = {
  1.00000000000000000000000000000,
  0.083333333333333333333333333333,
 -0.00138888888888888888888888888889,
  0.000033068783068783068783068783069,
 -8.2671957671957671957671957672e-07,
  2.0876756987868098979210090321e-08,
 -5.2841901386874931848476822022e-10,
  1.3382536530684678832826980975e-11,
 -3.3896802963225828668301953912e-13,
  8.5860620562778445641359054504e-15,
 -2.1748686985580618730415164239e-16,
  5.5090028283602295152026526089e-18,
 -1.3954464685812523340707686264e-19,
  3.5347070396294674716932299778e-21,
 -8.9535174270375468504026113181e-23
};

/*-*-*-*-*-*-*-*-*-*-*-* Functions with Error Codes *-*-*-*-*-*-*-*-*-*-*-*/

static int gsl_sf_hzeta_e(const double s, const double q, gsl_sf_result * result)
{
  /* CHECK_POINTER(result) */

  if(s <= 1.0 || q <= 0.0) {
    PLFIT_ERROR("s must be larger than 1.0 and q must be larger than zero", PLFIT_EINVAL);
  }
  else {
    const double max_bits = 54.0;
    const double ln_term0 = -s * log(q);  

    if(ln_term0 < GSL_LOG_DBL_MIN + 1.0) {
      PLFIT_ERROR("underflow", PLFIT_UNDRFLOW);
    }
    else if(ln_term0 > GSL_LOG_DBL_MAX - 1.0) {
      PLFIT_ERROR("overflow", PLFIT_OVERFLOW);
    }
    else if((s > max_bits && q < 1.0) || (s > 0.5*max_bits && q < 0.25)) {
      result->val = pow(q, -s);
      result->err = 2.0 * GSL_DBL_EPSILON * fabs(result->val);
      return PLFIT_SUCCESS;
    }
    else if(s > 0.5*max_bits && q < 1.0) {
      const double p1 = pow(q, -s);
      const double p2 = pow(q/(1.0+q), s);
      const double p3 = pow(q/(2.0+q), s);
      result->val = p1 * (1.0 + p2 + p3);
      result->err = GSL_DBL_EPSILON * (0.5*s + 2.0) * fabs(result->val);
      return PLFIT_SUCCESS;
    }
    else {
      /* Euler-Maclaurin summation formula 
       * [Moshier, p. 400, with several typo corrections]
       */
      const int jmax = 12;
      const int kmax = 10;
      int j, k;
      const double pmax  = pow(kmax + q, -s);
      double scp = s;
      double pcp = pmax / (kmax + q);
      double ans = pmax*((kmax+q)/(s-1.0) + 0.5);

      for(k=0; k<kmax; k++) {
        ans += pow(k + q, -s);
      }

      for(j=0; j<=jmax; j++) {
        double delta = hzeta_c[j+1] * scp * pcp;
        ans += delta;
        if(fabs(delta/ans) < 0.5*GSL_DBL_EPSILON) break;
        scp *= (s+2*j+1)*(s+2*j+2);
        pcp /= (kmax + q)*(kmax + q);
      }

      result->val = ans;
      result->err = 2.0 * (jmax + 1.0) * GSL_DBL_EPSILON * fabs(ans);
      return PLFIT_SUCCESS;
    }
  }
}

/*-*-*-*-*-*-*-*-*-* Functions w/ Natural Prototypes *-*-*-*-*-*-*-*-*-*-*/

double gsl_sf_hzeta(const double s, const double a)
{
  gsl_sf_result result;
  gsl_sf_hzeta_e(s, a, &result);
  return result.val;
}

/* End of Part imported from GSL */

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

#ifndef GSL_NAN
#define GSL_NAN 0.0/0.0
#endif

#define HSL_SF_HZETA_EULERMACLAURIN_SERIES_SHIFT 10
#define HSL_SF_HZETA_EULERMACLAURIN_SERIES_ORDER 14 /*32 */

#define HSL_SF_HZETA_SQR1_2PI 2.533029591058444286096986580243e-02 /* 1/(2Pi)^2 */

extern int hsl_sf_hzeta_deriv_e(const double s, const double q, gsl_sf_result * result) {

	/* CHECK_POINTER(result) */

	if ((s <= 1.0) || (q <= 0.0)) {
		PLFIT_ERROR("s must be larger than 1.0 and q must be larger than zero", PLFIT_EINVAL);
		}
	else {
		const double ln_hz_term0=-s*log(q);
		if (ln_hz_term0 < GSL_LOG_DBL_MIN+1.0) {
            PLFIT_ERROR("underflow", PLFIT_UNDRFLOW);
			}
		else if (GSL_LOG_DBL_MAX-1.0 < ln_hz_term0) {
            PLFIT_ERROR("overflow", PLFIT_OVERFLOW);
			}
		else { /* Euler-Maclaurin summation formula */
			const double qshift=HSL_SF_HZETA_EULERMACLAURIN_SERIES_SHIFT+q;
			const double inv_qshift=1.0/qshift;
			const double sqr_inv_qshift=inv_qshift*inv_qshift;
			const double inv_sm1=1.0/(s-1.0);
			const double pmax=pow(qshift,-s);
			const double lmax=log(qshift);
			double terms[HSL_SF_HZETA_EULERMACLAURIN_SERIES_SHIFT+HSL_SF_HZETA_EULERMACLAURIN_SERIES_ORDER+1]={GSL_NAN};
			double delta=GSL_NAN;
			double tscp=s;
			double scp=tscp;
			double pcp=pmax*inv_qshift;
			double lcp=lmax-1.0/s;
			double ratio=scp*pcp*lcp;
			double qs=GSL_NAN;
			size_t n=0;
			size_t j=0;
			double ans=0.0;
			double mjr=GSL_NAN;

			for(j=0,qs=q;j<HSL_SF_HZETA_EULERMACLAURIN_SERIES_SHIFT;++qs,++j) ans+=(terms[n++]=log(qs)*pow(qs,-s));
			ans+=(terms[n++]=0.5*lmax*pmax);
			ans+=(terms[n++]=pmax*qshift*inv_sm1*(lmax+inv_sm1));
			for(j=1;j<=HSL_SF_HZETA_EULERMACLAURIN_SERIES_ORDER;++j) {
				delta=hzeta_c[j]*ratio;
				ans+=(terms[n++]=delta);
				scp*=++tscp; lcp-=1.0/tscp;
				scp*=++tscp; lcp-=1.0/tscp;
				pcp*=sqr_inv_qshift;
				ratio=scp*pcp*lcp;
				if ((fabs(delta/ans)) < (0.5*GSL_DBL_EPSILON)) break;
				}
			ans=0.0; while (n) ans+=terms[--n];
			mjr=0.17*pow(HSL_SF_HZETA_SQR1_2PI,j)*ratio;

			result->val=-ans;
			result->err=2.0*((HSL_SF_HZETA_EULERMACLAURIN_SERIES_SHIFT+1.0)*GSL_DBL_EPSILON*fabs(ans)+mjr);
			return (PLFIT_SUCCESS);
			}
		}

	return (PLFIT_SUCCESS); }

double hsl_sf_hzeta_deriv(const double s, const double a)
{
  gsl_sf_result result;
  hsl_sf_hzeta_deriv_e(s, a, &result);
  return result.val;
}


extern
int hsl_sf_hzeta_deriv2_e(const double s, const double q, gsl_sf_result * result) {

	/* CHECK_POINTER(result) */

	if ((s <= 1.0) || (q <= 0.0)) {
		PLFIT_ERROR("s must be larger than 1.0 and q must be larger than zero", PLFIT_EINVAL);
		}
	else {
		const double ln_hz_term0=-s*log(q);
		if (ln_hz_term0 < GSL_LOG_DBL_MIN+1.0) {
			PLFIT_ERROR("underflow", PLFIT_UNDRFLOW);
			}
		else if (GSL_LOG_DBL_MAX-1.0 < ln_hz_term0) {
			PLFIT_ERROR("overflow", PLFIT_OVERFLOW);
			}
		else { /* Euler-Maclaurin summation formula */
			const double qshift=HSL_SF_HZETA_EULERMACLAURIN_SERIES_SHIFT+q;
			const double inv_qshift=1.0/qshift;
			const double sqr_inv_qshift=inv_qshift*inv_qshift;
			const double inv_sm1=1.0/(s-1.0);
			const double pmax=pow(qshift,-s);
			const double lmax=log(qshift);
			const double lmax_p_inv_sm1=lmax+inv_sm1;
			const double sqr_inv_sm1=inv_sm1*inv_sm1;
			const double sqr_lmax=lmax*lmax;
			const double sqr_lmax_p_inv_sm1=lmax_p_inv_sm1*lmax_p_inv_sm1;
			double terms[HSL_SF_HZETA_EULERMACLAURIN_SERIES_SHIFT+HSL_SF_HZETA_EULERMACLAURIN_SERIES_ORDER+1]={GSL_NAN};
			double delta=GSL_NAN;
			double tscp=s;
			double slcp=GSL_NAN;
			double plcp=GSL_NAN;
			double scp=tscp;
			double pcp=pmax*inv_qshift;
			double lcp=1.0/s-lmax;
			double sqr_lcp=lmax*(lmax-2.0/s);
			double ratio=scp*pcp*sqr_lcp;
			double qs=GSL_NAN;
			double lqs=GSL_NAN;
			size_t n=0;
			size_t j=0;
			double ans=0.0;
			double mjr=GSL_NAN;

			for(j=0,qs=q;j<HSL_SF_HZETA_EULERMACLAURIN_SERIES_SHIFT;++qs,++j) {
				lqs=log(qs);
				ans+=(terms[n++]=lqs*lqs*pow(qs,-s));
				}
			ans+=(terms[n++]=0.5*sqr_lmax*pmax);
			ans+=(terms[n++]=pmax*qshift*inv_sm1*(sqr_lmax_p_inv_sm1+sqr_inv_sm1));
			for(j=1;j<=HSL_SF_HZETA_EULERMACLAURIN_SERIES_ORDER;++j) {
				delta=hzeta_c[j]*ratio;
				ans+=(terms[n++]=delta);
				scp*=++tscp; slcp=plcp=1.0/tscp;
				scp*=++tscp; slcp+=1.0/tscp; plcp/=tscp;
				pcp*=sqr_inv_qshift;
				sqr_lcp+=2.0*(plcp+slcp*lcp);
				ratio=scp*pcp*sqr_lcp;
				if ((fabs(delta/ans)) < (0.5*GSL_DBL_EPSILON)) break;
				lcp+=slcp;
				}

			ans=0.0; while (n) ans+=terms[--n];
			mjr=0.17*pow(HSL_SF_HZETA_SQR1_2PI,j)*ratio;

			result->val=+ans;
			result->err=2.0*((HSL_SF_HZETA_EULERMACLAURIN_SERIES_SHIFT+1.0)*GSL_DBL_EPSILON*fabs(ans)+mjr);
			return (PLFIT_SUCCESS);
			}
		}

	return (PLFIT_SUCCESS); }

double hsl_sf_hzeta_deriv2(const double s, const double a)
{
  gsl_sf_result result;
  hsl_sf_hzeta_deriv2_e(s, a, &result);
  return result.val;
}

/* End of Part imported from HSL */
