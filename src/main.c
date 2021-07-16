/* vim:set ts=4 sw=4 sts=4 et: */
/* main.c
 *
 * Copyright (C) 2010-2012 Tamas Nepusz
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
#include <string.h>
#include <time.h>
#include "getopt.h"
#include "platform.h"
#include "plfit.h"

/* exit status code for incorrect input data as defined in sysexits.h (8.1) */
#ifndef EX_DATAERR
#define EX_DATAERR 65
#endif

typedef struct _cmd_options_t {
    double alpha_min;
    double alpha_step;
    double alpha_max;
    plfit_bool_t brief_mode;
    double divisor;
    plfit_bool_t finite_size_correction;
    plfit_bool_t force_continuous;
    plfit_bool_t print_moments;
    plfit_p_value_method_t p_value_method;
    double p_value_precision;
    unsigned long seed;
    plfit_bool_t use_seed;
    double xmin;
} cmd_options_t;

cmd_options_t opts;
plfit_mt_rng_t rng;

void show_version(FILE* f) {
    fprintf(f, "plfit " PLFIT_VERSION_STRING "\n");
    return;
}

void usage(char* argv[]) {
    show_version(stderr);

    fprintf(stderr, "\nUsage: %s [options] [infile1 ...]\n\n", argv[0]);
    fprintf(stderr,
            "Reads data points from each given input file and fits a power-law\n"
            "distribution to them, one by one, according to the method of\n"
            "Clauset, Shalizi and Newman. If no input files are given, the\n"
            "standard input will be processed.\n\n"
            "This implementation uses the L-BFGS optimization method to find\n"
            "the optimal alpha for a given xmin in the discrete case. If you\n"
            "want to use the legacy brute-force approach originally published\n"
            "in the above paper, use the -a switch.\n\n"
            "Options:\n"
            "    -h        shows this help message\n"
            "    -v        shows version information\n"
            "    -a RANGE  use legacy brute-force search for the optimal alpha\n"
            "              when a discrete power-law distribution is fitted.\n"
            "              RANGE must be in MIN:STEP:MAX format, the default\n"
            "              is 1.5:0.01:3.5.\n"
            "    -b        brief (but easily parseable) output format\n"
            "    -c        force continuous fitting even when every sample\n"
            "              is an integer\n"
            "    -D VALUE  divide each sample in the input data by VALUE to prevent\n"
            "              underflows when fitting discrete power-law distribution\n"
            "    -e EPS    try to provide a p-value with a precision of EPS when\n"
            "              the p-value is calculated using the exact method. The\n"
            "              default is 0.01.\n"
            "    -f        use finite-size correction\n"
            "    -m XMIN   use XMIN as the minimum value for x instead of searching\n"
            "              for the optimal value\n"
            "    -M        print the first four central moments (i.e. mean, variance,\n"
            "              skewness and kurtosis) of the input data to help\n"
            "              assessing the shape of the pdf it may have come from.\n"
            "    -p METHOD use METHOD to calculate the p-value. Must be one of\n"
            "              skip, approximate or exact. Default is skip.\n"
            "    -s SEED   use SEED to seed the random number generator\n"
    );
    return;
}

int parse_cmd_options(int argc, char* argv[], cmd_options_t* opts) {
    int c;

    opts->alpha_min  = 1.5;
    opts->alpha_step = 0;
    opts->alpha_max  = 3.5;
    opts->brief_mode = 0;
    opts->divisor = 1;
    opts->finite_size_correction = 0;
    opts->force_continuous = 0;
    opts->print_moments = 0;
    opts->p_value_method = PLFIT_P_VALUE_SKIP;
    opts->p_value_precision = 0.01;
    opts->seed = 0;
    opts->use_seed = 0;
    opts->xmin = -1;

    opterr = 0;

    while ((c = getopt(argc, argv, "a:bcD:e:fhm:Mp:ts:v")) != -1) {
        switch (c) {
            case 'a':
                if (sscanf(optarg, "%lf:%lf:%lf", &opts->alpha_min,
                            &opts->alpha_step, &opts->alpha_max) != 3) {
                    fprintf(stderr, "Invalid format for option `-a': %s\n", optarg);
                    return 1;
                }
                break;

            case 'b':           /* brief mode */
                opts->brief_mode = 1;
                break;

            case 'c':           /* force continuous fitting */
                opts->force_continuous = 1;
                break;

            case 'D':           /* divide the input data */
                if (!sscanf(optarg, "%lg", &opts->divisor) || opts->divisor <= 0) {
                    fprintf(stderr, "Invalid value for option `-%c'\n", optopt);
                    return 1;
                }
                break;

            case 'e':           /* specify the precision for p-value calculations */
                if (!sscanf(optarg, "%lg", &opts->p_value_precision) ||
                        opts->p_value_precision <= 0) {
                    fprintf(stderr, "Invalid value for option `-%c'\n", optopt);
                    return 1;
                }
                break;

            case 'f':           /* finite size correction */
                opts->finite_size_correction = 1;
                break;

            case 'h':           /* shows help */
                usage(argv);
                return 0;

            case 'm':           /* specify xmin explicitly */
                if (!sscanf(optarg, "%lg", &opts->xmin) || opts->xmin < 0) {
                    fprintf(stderr, "Invalid value for option `-%c'\n", optopt);
                    return 1;
                }
                break;

            case 'M':           /* print the moments of the distribution */
                opts->print_moments = 1;
                break;

            case 'p':           /* p-value method */
                if (!strcmp(optarg, "none") || !strcmp(optarg, "skip")) {
                    opts->p_value_method = PLFIT_P_VALUE_SKIP;
                } else if (!strcmp(optarg, "approximate") || !strcmp(optarg, "approx")) {
                    opts->p_value_method = PLFIT_P_VALUE_APPROXIMATE;
                } else if (!strcmp(optarg, "exact")) {
                    opts->p_value_method = PLFIT_P_VALUE_EXACT;
                } else {
                    fprintf(stderr, "Invalid value for option `-%c': %s\n", optopt,
                            optarg);
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

            case '?':           /* unknown option */
                if (optopt == 'a' || optopt == 'm')
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

    return -1;
}

void process_file(FILE* f, const char* fname) {
    double* data;
    size_t i, n = 0, nalloc = 100;
    unsigned short int warned = 0, discrete = opts.force_continuous ? 0 : 1;
	plfit_continuous_options_t plfit_continuous_options;
	plfit_discrete_options_t plfit_discrete_options;
    plfit_result_t result;
    struct {
        double mean;
        double variance;
        double skewness;
        double kurtosis;
    } moments = { 0, 0, 0, 0 };

    /* allocate memory for 100 samples */
    data = (double*)calloc(nalloc, sizeof(double));
    if (data == 0) {
        perror(fname);
        return;
    }

    /* read the input file */
    for (;;) {
        int nparsed = fscanf(f, "%lf", data+n);
        if (nparsed == EOF)  /* reached the end of file */
            break;

        if (nparsed < 1) {   /* parse error */
            int c = '\0';
            if ((c = fgetc(f)) == '#') {
                do { c = fgetc(f); } while (c != '\n');
            } else {
                if (warned++ < 16) {
                    fprintf(stderr, "%s: parse error at byte %ld\n", fname, (long)ftell(f));
                }
            }
            continue;
        }

        if (discrete && (floor(data[n]) != data[n]))
            discrete = 0;

        n++;
        if (n == nalloc) {
            /* allocate twice as much memory */
            nalloc *= 2;
            data = (double*)realloc(data, sizeof(double) * nalloc);
            if (data == 0) {
                perror(fname);
                return;
            }
        }
    }

    if (warned) {
        fprintf(stderr, "%s: corrupted data points in file\n", fname);
        exit(EX_DATAERR);
        return;
    }
    if (n == 0) {
        fprintf(stderr, "%s: no data points in file\n", fname);
        exit(EX_DATAERR);
        return;
    }

    /* apply the divisor if needed */
    if (opts.divisor != 1) {
#ifdef _OPENMP
#pragma omp parallel for private(i)
#endif
        for (i = 0; i < n; i++) {
            data[i] /= opts.divisor;
        }
        if (discrete) {
#ifdef _OPENMP
#pragma omp parallel for private(i)
#endif
            for (i = 0; i < n; i++) {
                data[i] = round(data[i]);
            }
        }
    }

	/* construct the plfit options */
	plfit_continuous_options_init(&plfit_continuous_options);
	plfit_discrete_options_init(&plfit_discrete_options);
	plfit_continuous_options.finite_size_correction = opts.finite_size_correction;
	plfit_discrete_options.finite_size_correction = opts.finite_size_correction;
    plfit_continuous_options.p_value_method = opts.p_value_method;
    plfit_discrete_options.p_value_method = opts.p_value_method;
    plfit_continuous_options.p_value_precision = opts.p_value_precision;
    plfit_discrete_options.p_value_precision = opts.p_value_precision;
    plfit_continuous_options.rng = &rng;
    plfit_discrete_options.rng = &rng;

    /* fit the power-law distribution */
    if (discrete) {
        if (opts.alpha_step > 0) {
            /* Old estimation based on brute-force search */
			plfit_discrete_options.alpha_method = PLFIT_LINEAR_SCAN;
			plfit_discrete_options.alpha.min = opts.alpha_min;
			plfit_discrete_options.alpha.max = opts.alpha_max;
			plfit_discrete_options.alpha.step = opts.alpha_step;
        } else {
			plfit_discrete_options.alpha_method = PLFIT_LBFGS;
		}
		if (opts.xmin < 0) {
			/* Estimate xmin and alpha */
			plfit_discrete(data, n, &plfit_discrete_options, &result);
		} else {
			/* Estimate alpha only */
			plfit_estimate_alpha_discrete(data, n, opts.xmin,
					&plfit_discrete_options, &result);
		}
    } else {
        if (opts.xmin < 0) {
            /* Estimate xmin and alpha */
            plfit_continuous(data, n, &plfit_continuous_options, &result);
        } else {
            /* Estimate alpha only */
            plfit_estimate_alpha_continuous(data, n, opts.xmin,
                    &plfit_continuous_options, &result);
        }
    }

    /* calculate the moments if needed */
    if (opts.print_moments) {
        plfit_moments(data, n, &moments.mean, &moments.variance,
                &moments.skewness, &moments.kurtosis);
    }

    /* print the results */
    if (opts.brief_mode) {
        if (opts.print_moments) {
            printf("%s: S %lg %lg %lg %lg\n", fname, moments.mean, moments.variance,
                    moments.skewness, moments.kurtosis);
        }
        printf("%s: %c %lg %lg %lg %lg %lg\n", fname, discrete ? 'D' : 'C',
                result.alpha, result.xmin, result.L, result.D, result.p);
    } else {
        printf("%s:\n", fname);
        if (!opts.finite_size_correction && n < 50)
            printf("\tWARNING: finite size bias may be present\n\n");

        if (opts.print_moments) {
            printf("\tCentral moments\n");
            printf("\tmean     = %12.5lf\n", moments.mean);
            printf("\tvariance = %12.5lf\n", moments.variance);
            printf("\tstd.dev. = %12.5lf\n", sqrt(moments.variance));
            printf("\tskewness = %12.5lf\n", moments.skewness);
            printf("\tkurtosis = %12.5lf\n", moments.kurtosis);
            printf("\tex.kurt. = %12.5lf\n", moments.kurtosis-3);
            printf("\n");
        }

        printf("\t%s MLE", discrete ? "Discrete" : "Continuous");
        if (opts.finite_size_correction)
            printf(" with finite size correction");
        printf("\n");
        printf("\talpha = %12.5lf\n", result.alpha);
        printf("\txmin  = %12.5lf\n", result.xmin );
        printf("\tL     = %12.5lf\n", result.L    );
        printf("\tD     = %12.5lf\n", result.D    );
        if (!isnan(result.p)) {
            printf("\tp     = %12.5lf%s\n", result.p,
                    opts.p_value_method == PLFIT_P_VALUE_APPROXIMATE ?
                    " (approximation)" : "");
        }
        printf("\n");
    }

    /* free the stored data */
    free(data);
}

int main(int argc, char* argv[]) {
    int result = parse_cmd_options(argc, argv, &opts);
    int i, retval;

    if (result != -1)
        return result;

    srand(opts.use_seed ? opts.seed : ((unsigned int)time(0)));
    plfit_mt_init(&rng);

    retval = 0;
    if (optind == argc) {
        /* no more arguments, process stdin */
        process_file(stdin, "-");
    } else {
        for (i = optind; i < argc; i++) {
            FILE* f;

            if (argv[i][0] == '-')
                f = stdin;
            else
                f = fopen(argv[i], "r");

            if (!f) {
                perror(argv[i]);
                retval = 2;
                continue;
            }

            process_file(f, argv[i]);
            fclose(f);
        }
    }

    return retval;
}
