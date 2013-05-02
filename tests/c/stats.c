/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Statistical routines for testing RNG.
 *
 * Chi-square p-value calculation by Jacob F. W. at the Code Project:
 * http://www.codeproject.com/Articles/432194/How-to-Calculate-the-Chi-Squared-P-Value
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "ojrandlib.h"

typedef struct {
    int n;
    double mn, mx, r;
    long total;
    long *counts;
} counter;

/* Stirling's approximation of the Gamma function.
 */

static const double ONE_OVER_E = 0.3678794411714423;
static const double TWO_PI = 6.2831853071795864;

static double stirling(double z) {
    double d = 1.0 / (10.0 * z);
    d = 1.0 / ((12 * z) - d);
    d = (d + z) * ONE_OVER_E;
    d = pow(d, z);
    return d * sqrt(TWO_PI / z);
}

/* Incomplete gamma function.
 */

static double igf(double s, double z) {
    int i;
    double sc, sum = 1.0, num = 1.0, denom = 1.0;
    if (z < 0.0) return 0.0;

    sc = (1.0 / s);
    sc *= pow(z, s);
    sc *= exp(-z);

    for(i = 0; i < 200; ++i) {
        num *= z;
        ++s;
        denom *= s;
        sum += (num / denom);
    }
    return sum * sc;
}

/* Chi-square p-value.
 */

static double chi2p(int df, double cv) {
    double k, x, p;
    if (cv < 0 || df < 1) return 0.0;

    k = ((double)df) * 0.5;
    x = cv * 0.5;
    if (2 == df) return exp(-x);

    p = igf(k, x);
    if (isnan(p) || isinf(p) || p <= 1e-8) return 1e-14;

    p /= stirling(k);
    return 1.0 - p;
}

counter *counter_open(int n, double mn, double mx) {
    assert(n > 0 && mn != mx);
    counter *c = malloc(sizeof(counter));
    if (c) {
        if (! (c->counts = calloc(n, sizeof(long)))) {
            free(c);
            return NULL;
        }
        c->n = n; c->mn = mn; c->mx = mx;
        c->r = mx - mn;
        c->total = 0;
    }
    return c;
}

void counter_close(counter *c) {
    free(c->counts);
    free(c);
}

void counter_inc_b(counter *c, int b) {
    assert(c && b >= 0 && b < c->n);

    ++c->counts[b];
    ++c->total;
}

void counter_inc_f(counter *c, double v) {
    int b;
    assert(c);

    b = (int)(((v - c->mn) * c->n) / c->r);
    assert(b >= 0 && b < c->n);

    ++c->counts[b];
    ++c->total;
}

double counter_print_stats(counter *c) {
    int i;
    double d, d2, p, m, sd, t = 0.0, chi2 = 0.0;

    m = (double)(c->total) / c->n;
    for (i = 0; i < c->n; ++i) {
        d = (double)(c->counts[i]) - m;
        d2 = d * d;
        t += d2;
        chi2 += d2 / m;
    }
    sd = sqrt(t / (double)(c->n - 1));
    p = chi2p(c->n - 1, chi2);

    printf("%4d buckets, mean = %12g, stdev = %12g, p-value = %7.4f\n",
        c->n, m, sd, p);
    return p;
}

int main(int argc, char *argv[]) {
    int i, r;
    counter *c = counter_open(52, 0.0, 52.0);
    ojr_generator *g = ojr_open(0);

    ojr_good_seed(g);
    for (i = 0; i < 1000000000; ++i) {
        // r = ojr_next16(g) % 52;
        r = ojr_rand(g, 52);
        counter_inc_b(c, r);
    }
    counter_print_stats(c);

    ojr_close(g);
    counter_close(c);
    return EXIT_SUCCESS;
}
