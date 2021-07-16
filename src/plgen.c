/* plgen.c
 *
 * Copyright (C) 2012 Tamas Nepusz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
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

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "getopt.h"
#include "plfit.h"
#include "plfit_sampling.h"

typedef struct _cmd_options_t {
    long int num_samples;

    double gamma;
    double kappa;
    double offset;
    double xmin;

    plfit_bool_t continuous;

    plfit_bool_t use_seed;
    unsigned long seed;
} cmd_options_t;

cmd_options_t opts;
plfit_mt_rng_t rng;

void show_version(FILE* f) {
    fprintf(f, "plgen " PLFIT_VERSION_STRING "\n");
    return;
}

void usage(char* argv[]) {
    show_version(stderr);

    fprintf(stderr, "\nUsage: %s [options] num_samples gamma [kappa]\n\n", argv[0]);
    fprintf(stderr,
            "Generates a given number of samples from a power-law distribution\n"
            "with an optional exponential cutoff. The pdf being sampled is given\n"
            "as follows:\n\n"
            "P(k) = C * k^(-gamma) * exp(-k/kappa)\n\n"
            "where C is an appropriate normalization constant. gamma is given by\n"
            "the second command line argument, kappa is given by the -k switch.\n\n"
            "Options:\n"
            "    -h         shows this help message\n"
            "    -v         shows version information\n"
            "    -c         generate continuous samples\n"
            "    -k KAPPA   use exponential cutoff with kappa = KAPPA\n"
            "    -o OFFSET  add OFFSET to each generated sample\n"
            "    -s SEED    use SEED to seed the random number generator\n"
            "    -x XMIN    generate a power-law distribution that starts at XMIN\n"
    );
    return;
}

int parse_cmd_options(int argc, char* argv[], cmd_options_t* opts) {
    int c;

    opts->offset = 0;
    opts->continuous = 0;
    opts->kappa = -1;
    opts->seed = 0;
    opts->use_seed = 0;
    opts->xmin = 1;

    opterr = 0;

    while ((c = getopt(argc, argv, "chk:o:s:vx:")) != -1) {
        switch (c) {
            case 'c':           /* use continuous samples */
                opts->continuous = 1;
                break;

            case 'h':           /* shows help */
                usage(argv);
                return 0;

            case 'k':           /* use exponential cutoff */
                if (!sscanf(optarg, "%lg", &opts->kappa)) {
                    fprintf(stderr, "Invalid value for option `-%c'\n", optopt);
                    return 1;
                }
                break;

            case 'o':           /* specify offset explicitly */
                if (!sscanf(optarg, "%lg", &opts->offset)) {
                    fprintf(stderr, "Invalid value for option `-%c'\n", optopt);
                    return 1;
                }
                break;

            case 's':           /* set random seed */
                if (!sscanf(optarg, "%lu", &opts->seed)) {
                    fprintf(stderr, "Invalid value for option `-%c'\n", optopt);
                    return 1;
                }
                opts->use_seed = 1;
                break;

            case 'v':           /* version information */
                show_version(stdout);
                return 0;

            case 'x':           /* specify xmin explicitly */
                if (!sscanf(optarg, "%lg", &opts->xmin)) {
                    fprintf(stderr, "Invalid value for option `-%c'\n", optopt);
                    return 1;
                }
                if (opts->xmin <= 0) {
                    fprintf(stderr, "Value of option `-%c' must be positive\n",
                            optopt);
                    return 1;
                }
                break;

            case '?':           /* unknown option */
                if (optopt == 'o' || optopt == 's' || optopt == 'k')
                    fprintf(stderr, "Option `-%c' requires an argument\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Invalid option `-%c'\n", optopt);
                else
                    fprintf(stderr, "Invalid option character `\\x%x'.\n", optopt);
                return 1;

            default:
                abort();
        }
    }

    if (!opts->continuous && ceil(opts->xmin) != opts->xmin) {
        fprintf(stderr, "WARNING: xmin is rounded up to the nearest integer\n");
        opts->xmin = ceil(opts->xmin);
    }

    return -1;
}

#define BLOCK_SIZE 16384

int sample_discrete() {
    long int samples[BLOCK_SIZE];
    double* probs;
    size_t num_probs;
    plfit_walker_alias_sampler_t sampler;
    size_t i, n;
    double u;

    if (opts.num_samples <= 0)
        return 0;

    if (opts.kappa > 0) {
        /* Power law with exponential cutoff. We use Walker's algorithm here. */

        /* Construct probability array */
        num_probs = 100000;
        probs = (double*)calloc(num_probs, sizeof(double));
        if (probs == 0) {
            fprintf(stderr, "Not enough memory\n");
            return 7;
        }

#ifdef _OPENMP
#pragma omp parallel for private(i)
#endif
        for (i = 0; i < num_probs; i++) {
            probs[i] = exp(-i / opts.kappa) * pow((i + opts.xmin) / opts.xmin, -opts.gamma);
        }

        /* Initialize sampler */
        if (plfit_walker_alias_sampler_init(&sampler, probs, num_probs)) {
            fprintf(stderr, "Error while initializing sampler\n");
            free(probs);
            return 9;
        }

        /* Free "probs" array */
        free(probs);

        /* Sampling */
        while (opts.num_samples > 0) {
            n = opts.num_samples > BLOCK_SIZE ? BLOCK_SIZE : opts.num_samples;
            plfit_walker_alias_sampler_sample(&sampler, samples, n, &rng);

            for (i = 0; i < n; i++) {
                printf("%ld\n", (long int)(samples[i] + opts.offset + opts.xmin));
            }

            opts.num_samples -= n;
        }

        /* Destroy sampler */
        plfit_walker_alias_sampler_destroy(&sampler);

    } else {
        /* Pure power law */
        for (i = 0; i < opts.num_samples; i++) {
            u = plfit_rzeta((long int)opts.xmin, opts.gamma, &rng) + opts.offset;
            printf("%ld\n", (long int)u);
        }
    }
    return 0;
}

int sample_continuous() {
    long int i;
    double u;

    if (opts.kappa > 0) {
        fprintf(stderr, "Exponential cutoff not supported for continuous sampling, sorry.\n");
        return 5;
    }

    for (i = 0; i < opts.num_samples; i++) {
        u = plfit_rpareto(opts.xmin, opts.gamma-1, &rng) + opts.offset;
        printf("%.8f\n", u);
    }

    return 0;
}

int main(int argc, char* argv[]) {
    int result = parse_cmd_options(argc, argv, &opts);
    int retval;

    if (result != -1)
        return result;

    retval = 0;
    if (argc - optind < 2) {
        /* not enough arguments */
        usage(argv);
        retval = 2;
    } else {
        if (!sscanf(argv[optind], "%ld", &opts.num_samples)) {
            fprintf(stderr, "Format of num_samples parameter is invalid.\n");
            retval = 3;
        } else if (!sscanf(argv[optind+1], "%lg", &opts.gamma)) {
            fprintf(stderr, "Format of gamma parameter is invalid.\n");
            retval = 4;
        } else {
            srand(opts.use_seed ? opts.seed : ((unsigned int)time(0)));
            plfit_mt_init(&rng);
            retval = opts.continuous ? sample_continuous() : sample_discrete();
        }
    }

    return retval;
}
