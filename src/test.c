#include <stdio.h>
#include <math.h>
#include "kolmogorov.h"
#include "plfit.h"

#define RUN_TEST_CASE(func) do { \
	int result; \
	result = func(); \
	if (result != 0) return result; \
} while (0)
#define ASSERT_ALMOST_EQUAL(obs, exp, eps) do { \
	double diff = (obs) - (exp); \
	if (isnan(diff) || diff < -eps || diff > eps) { \
		fprintf(stderr, "%s:%d : expected %.8f, got %.8f\n", \
				__FILE__, __LINE__, (double)exp, (double)obs); \
		return 1; \
	} \
} while (0)
#define ASSERT_EQUAL(obs, exp) do { \
	if (obs != exp) { \
		fprintf(stderr, "%s:%d : expected %.8f, got %.8f\n", \
				__FILE__, __LINE__, (double)exp, (double)obs); \
		return 1; \
	} \
} while (0)

int test_read_file(const char* fname, double* data, int max_n) {
	FILE* f = fopen(fname, "r");
	int n = 0;

	if (!f)
		return 0;

	while (!feof(f) && n < max_n) {
		if (fscanf(f, "%lf", data+n)) {
			n++;
		}
	}

	fclose(f);
	return n;
}

int test_kolmogorov() {
	ASSERT_ALMOST_EQUAL(plfit_ks_test_two_sample_p(0.1733, 50, 30), 0.574, 1e-2);
	return 0;
}

int test_continuous() {
	double alpha, xmin, L;
	double data[10000];
	int n;

	n = test_read_file("../data/continuous_data.txt", data, 10000);

	alpha = 2.53282;
	xmin = 1.43628;
	plfit_log_likelihood_continuous(data, n, alpha, xmin, &L);
	ASSERT_ALMOST_EQUAL(L, -9276.42, 1e-1);

	alpha = 0;
	xmin = 1.43628;
	plfit_estimate_alpha_continuous(data, n, xmin, &alpha);
	ASSERT_ALMOST_EQUAL(alpha, 2.53282, 1e-4);

	alpha = xmin = 0;
	plfit_continuous(data, n, &alpha, &xmin);
	ASSERT_ALMOST_EQUAL(alpha, 2.53282, 1e-4);
	ASSERT_ALMOST_EQUAL(xmin, 1.43628, 1e-4);

	return 0;
}

int test_discrete() {
	double alpha, xmin, L;
	double data[10000];
	int n;

	n = test_read_file("../data/discrete_data.txt", data, 10000);

	alpha = 2.58;
	xmin = 2;
	plfit_log_likelihood_discrete(data, n, alpha, xmin, &L);
	ASSERT_ALMOST_EQUAL(L, -9155.62809, 1e-4);

	alpha = 0;
	xmin = 2;
	plfit_estimate_alpha_discrete_fast(data, n, xmin, &alpha);
	ASSERT_ALMOST_EQUAL(alpha, 2.43, 1e-1);

	alpha = 0;
	xmin = 2;
	plfit_estimate_alpha_discrete(data, n, xmin, &alpha);
	ASSERT_ALMOST_EQUAL(alpha, 2.58, 1e-1);

	alpha = 0;
	xmin = 0;
	plfit_discrete(data, n, &alpha, &xmin);
	ASSERT_ALMOST_EQUAL(alpha, 2.58, 1e-1);
	ASSERT_EQUAL(xmin, 2);

	return 0;
}

int test() {
	RUN_TEST_CASE(test_kolmogorov);
	RUN_TEST_CASE(test_continuous);
	RUN_TEST_CASE(test_discrete);

	return 0;
}
