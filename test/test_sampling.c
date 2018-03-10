/* test_sampling.c
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

#include <math.h>
#include "sampling.h"
#include "test_common.h"

#define NUM_SAMPLES 1000000
#define NUM_ITEMS 26

int test_sampling(void) {
    plfit_walker_alias_sampler_t sampler;
    double probs[NUM_ITEMS];
    double prob_sum = 0.0;
    long int data[NUM_SAMPLES];
    long int hist[NUM_ITEMS];
    long int i, j, max_hist;
    plfit_mt_rng_t rng;

    plfit_mt_init(&rng);

    /* Create the sampler, sample, destroy */
    for (i = 0; i < NUM_ITEMS; i++) {
        probs[i] = i;
        prob_sum += probs[i];
    }
    if (plfit_walker_alias_sampler_init(&sampler, probs, NUM_ITEMS)) {
        return 1;
    }
    if (plfit_walker_alias_sampler_sample(&sampler, data, NUM_SAMPLES, &rng)) {
        return 2;
    }
    plfit_walker_alias_sampler_destroy(&sampler);

    /* Calculate histogram */
    for (i = 0; i < NUM_ITEMS; i++) {
        hist[i] = 0;
    }
    for (i = 0; i < NUM_SAMPLES; i++) {
        hist[data[i]]++;
    }
    for (i = 0, max_hist = 0; i < NUM_ITEMS; i++) {
        if (hist[i] > max_hist) {
            max_hist = hist[i];
        }
    }

    /* Print and test histogram */
    for (i = 0; i < NUM_ITEMS; i++) {
        printf("%3d: ", (int)i);
        j = (int)(hist[i] / ((double)max_hist) * 70);
        while (j > 0) {
            printf("#");
            j--;
        }
        printf("\n");

        ASSERT_WITHIN_RANGE(hist[i],
                (probs[i]/prob_sum - 0.05) * NUM_SAMPLES,
                (probs[i]/prob_sum + 0.05) * NUM_SAMPLES);
    }

    return 0;
}

int main(int argc, char* argv[]) {
    return test_sampling();
}

