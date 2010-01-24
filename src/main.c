#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "getopt.h"
#include "plfit.h"

#define VERSION "0.1"

extern int test();

typedef struct _cmd_options_t {
	double alpha_min;
	double alpha_step;
	double alpha_max;
} cmd_options_t;

cmd_options_t opts;

void show_version(FILE* f) {
	fprintf(f, "plfit " VERSION "\n");
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
            "Options:\n"
            "    -h        shows this help message\n"
            "    -v        shows version information\n"
			"    -a RANGE  specifies the range of power-law exponents to try\n"
			"              when a discrete power-law distribution is fitted.\n"
			"              RANGE must be in MIN:STEP:MAX format, the default\n"
			"              is 1.5:0.01:3.5.\n"
	);
	return;
}

int parse_cmd_options(int argc, char* argv[], cmd_options_t* opts) {
	int c;

	opts->alpha_min  = 1.5;
	opts->alpha_step = 0.01;
	opts->alpha_max  = 3.5;

	opterr = 0;

	while ((c = getopt(argc, argv, "a:htv")) != -1) {
		switch (c) {
			case 'a':
				if (sscanf(optarg, "%lf:%lf:%lf", &opts->alpha_min,
							&opts->alpha_step, &opts->alpha_max) != 3) {
					fprintf(stderr, "Invalid format for option `-a': %s\n", optarg);
					return 1;
				}
				break;

			case 'h':           /* shows help */
				usage(argv);
				return 0;

			case 't':           /* self-test */
				return test();

			case 'v':           /* version information */
				show_version(stdout);
				return 0;

			case '?':           /* unknown option */
				if (optopt == 'a')
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
	size_t n = 0, nalloc = 100;
	unsigned short int warned = 0, discrete = 1;
	plfit_result_t result;

	/* allocate memory for 100 samples */
	data = (double*)calloc(nalloc, sizeof(double));
	if (data == 0) {
		perror(fname);
		return;
	}

	/* read the input file */
	while (1) {
		int nparsed = fscanf(f, "%lf", data+n);
		if (nparsed == EOF)  /* reached the end of file */
			break;

		if (nparsed < 1) {   /* parse error */
			if (!warned) {
				fprintf(stderr, "%s: parse error at byte %ld\n", fname, (long)ftell(f));
				warned = 1;
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

	if (n == 0) {
		fprintf(stderr, "%s: no data points in file\n", fname);
		return;
	}

	/* fit the power-law distribution */
	if (discrete)
		plfit_discrete_in_range(data, n, opts.alpha_min, opts.alpha_max, opts.alpha_step, &result);
	else
		plfit_continuous(data, n, &result);

	/* print the results */
	printf("%s: %lg %lg %lg %lg\n", fname, result.alpha, result.xmin, result.L, result.p);

	/* free the stored data */
	free(data);
}

int main(int argc, char* argv[]) {
	int result = parse_cmd_options(argc, argv, &opts);
	int i, retval;

	if (result != -1)
		return result;

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
