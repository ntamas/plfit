/* sampling.c
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

#include "error.h"
#include "sampling.h"

int plfit_walker_alias_sampler_init(plfit_walker_alias_sampler_t* sampler,
        double* ps, size_t n) {
    double *p, *p2, *ps_end;
    double sum;
    long int *short_sticks, *long_sticks;
    long int num_short_sticks, num_long_sticks;
    long int i;
    
    sampler->num_bins = n;

    ps_end = ps + n;

    /* Initialize indexes and probs */
    sampler->indexes = (long int*)calloc(n, sizeof(long int));
    if (sampler->indexes == 0) {
        return PLFIT_ENOMEM;
    }
    sampler->probs   = (double*)calloc(n, sizeof(double));
    if (sampler->probs == 0) {
        free(sampler->indexes);
        return PLFIT_ENOMEM;
    }

    /* Normalize the probability vector; count how many short and long sticks
     * are there initially */
    for (sum = 0.0, p = ps; p != ps_end; p++) {
        sum += *p;
    }
    sum = n / sum;

    num_short_sticks = num_long_sticks = 0;
    for (p = ps, p2 = sampler->probs; p != ps_end; p++, p2++) {
        *p2 = *p * sum;
        if (*p2 < 1) {
            num_short_sticks++;
        } else if (*p2 > 1) {
            num_long_sticks++;
        }
    }

    /* Allocate space for short & long stick indexes */
    long_sticks = (long int*)calloc(num_long_sticks, sizeof(long int));
    if (long_sticks == 0) {
        free(sampler->probs);
        free(sampler->indexes);
        return PLFIT_ENOMEM;
    }
    short_sticks = (long int*)calloc(num_long_sticks, sizeof(long int));
    if (short_sticks == 0) {
        free(sampler->probs);
        free(sampler->indexes);
        free(long_sticks);
        return PLFIT_ENOMEM;
    }

    /* Initialize short_sticks and long_sticks */
    num_short_sticks = num_long_sticks = 0;
    for (i = 0, p = sampler->probs; i < n; i++, p++) {
        if (*p < 1) {
            short_sticks[num_short_sticks++] = i;
        } else if (*p > 1) {
            long_sticks[num_long_sticks++] = i;
        }
    }

    /* Prepare the index table */
    while (num_short_sticks && num_long_sticks) {
        long int short_index, long_index;
        short_index = short_sticks[--num_short_sticks];
        long_index = long_sticks[num_long_sticks-1];
        sampler->indexes[short_index] = long_index;
        sampler->probs[long_index] -= (1 - sampler->probs[short_index]);
        if (sampler->probs[long_index] < 1) {
            short_sticks[num_short_sticks++] = long_index;
            num_long_sticks--;
        }
    }

    return PLFIT_SUCCESS;
}


void plfit_walker_alias_sampler_destroy(plfit_walker_alias_sampler_t* sampler) {
    if (sampler->indexes) {
        free(sampler->indexes);
        sampler->indexes = 0;
    }
    if (sampler->probs) {
        free(sampler->probs);
        sampler->probs = 0;
    }
}


int plfit_walker_alias_sampler_sample(const plfit_walker_alias_sampler_t* sampler,
        long int *xs, size_t n) {
    double u;
    long int j;
    long int *x;

    x = xs;
    while (n > 0) {
        u = rand() / ((double)RAND_MAX);
        j = rand() % sampler->num_bins;
        *x = (u < sampler->probs[j]) ? j : sampler->indexes[j];
        n--; x++;
    }

    return PLFIT_SUCCESS;
}

