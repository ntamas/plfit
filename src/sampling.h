/* sampling.h
 * 
 * Copyright (C) 2012 Tamas Nepusz
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

#ifndef __SAMPLING_H__
#define __SAMPLING_H__

#include <stdlib.h>
#include "mt.h"

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

/**
 * Random sampler using Walker's alias method.
 */
typedef struct {
    long int num_bins;            /**< Number of bins */
    long int* indexes;            /**< Index of the "other" element in each bin */
    double* probs;                /**< Probability of drawing the "own" element from a bin */
} plfit_walker_alias_sampler_t;

/**
 * \brief Initializes the sampler with item probabilities.
 *
 * \param  sampler  the sampler to initialize
 * \param  ps   pointer to an array containing a value proportional to the
 *              sampling probability of each item in the set being sampled.
 * \param  n    the number of items in the array
 * \return error code
 */
int plfit_walker_alias_sampler_init(plfit_walker_alias_sampler_t* sampler,
        double* ps, size_t n);

/**
 * \brief Destroys an initialized sampler and frees the allocated memory.
 *
 * \param  sampler  the sampler to destroy
 */
void plfit_walker_alias_sampler_destroy(plfit_walker_alias_sampler_t* sampler);

/**
 * \brief Draws a given number of samples from the sampler and writes them
 *        to a given array.
 *
 * \param  sampler  the sampler to use
 * \param  xs       pointer to an array where the sampled items should be
 *                  written
 * \param  n        the number of samples to draw
 * \param  rng      the Mersenne Twister random number generator to use
 * \return error code
 */
int plfit_walker_alias_sampler_sample(const plfit_walker_alias_sampler_t* sampler,
        long int* xs, size_t n, mt_rng_t* rng);

__END_DECLS

#endif

