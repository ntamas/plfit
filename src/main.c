#include <stdio.h>
#include "plfit.h"

#define RUN_TEST_CASE(func) do { \
	int result; \
	result = func(); \
	if (result != 0) return result; \
} while (0)
#define ASSERT_ALMOST_EQUAL(obs, exp, eps) do { \
	double diff = (obs) - (exp); \
	if (diff < -eps || diff > eps) { \
		fprintf(stderr, "%s:%d : expected %.8f, got %.8f\n", \
				__FILE__, __LINE__, exp, obs); \
		return 1; \
	} \
} while (0)

int test_continuous() {
	double data[10000];
	double alpha, xmin, L;
	int n = 0;
	FILE* f = fopen("../data/continuous_data.txt", "r");

	while (!feof(f)) {
		if (fscanf(f, "%lf", data+n)) {
			n++;
		}
	}

	xmin = 1.43628;
	plfit_estimate_alpha_continuous(data, n, xmin, &alpha);
	ASSERT_ALMOST_EQUAL(alpha, 2.53282, 1e-4);
	plfit_log_likelihood_continuous(data, n, alpha, xmin, &L);
	ASSERT_ALMOST_EQUAL(L, -9276.42, 1e-1);

	alpha = xmin = 0;
	plfit_continuous(data, n, &alpha, &xmin);
	ASSERT_ALMOST_EQUAL(alpha, 2.53282, 1e-4);
	ASSERT_ALMOST_EQUAL(xmin, 1.43628, 1e-4);

	return 0;
}

int test() {
	RUN_TEST_CASE(test_continuous);
}

int main(int argc, char* argv[]) {
	return test();
}
