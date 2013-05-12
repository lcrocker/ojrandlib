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
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include <gsl/gsl_sf_gamma.h>
#include "ojrandlib.h"

typedef struct {
    int n;
    double mn, mx, r;
    long total;
    long *counts;
} counter;

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
    p = gsl_sf_gamma_inc_Q(0.5 *(c->n - 1), 0.5 * chi2);

    printf("%4d buckets,  mean =%9g,  stdev =%8g,  p-value =%6.3f\n",
        c->n, m, sd, p);
    return p;
}

int balance(char *alg, int n, int count) {
    counter *c;
    double p;
    ojr_generator *g = ojr_open(alg);
    ojr_system_seed(g);

    for (int i = 0; i < 3; ++i) {
        if (2 == i) c = counter_open(n, -1.0, 1.0);
        else c = counter_open(n, 0.0, 1.0);

        switch (i) {
        case 0:
            for (int j = 0; j < count; ++j) counter_inc_b(c, ojr_rand(g, n));
            break;
        case 1:
            for (int j = 0; j < count; ++j) counter_inc_f(c, ojr_next_double(g));
            break;
        case 2:
            for (int j = 0; j < count; ++j) counter_inc_f(c, ojr_next_signed_double(g));
            break;
        }
        printf("%10s (%c): ", alg, "ifs"[i]);
        p = counter_print_stats(c);
        counter_close(c);
        // if (p < 0.01) break;
    }
    ojr_close(g);

    // if (p < 0.01) return 10;
    // else
    return 0;
}

static char *anames[] = { "mwc256", "jkiss", "mt19937" };
static int bsizes[] = { 7, 52, 65, 256, 1000 };

int balance_set(void) {
    int f = 0;

    for (int i = 0; i < sizeof(anames)/ sizeof(anames[0]); ++i) {
        for (int j = 0; j < sizeof(bsizes) / sizeof(bsizes[0]); ++j) {
            f = balance(anames[i], bsizes[j], 5000000);
            if (f) return f;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int f = 0;
    char *alg;

    if (argc > 1 && (0 == strcmp("-d", argv[1]))) {
        if (argc > 2) alg = argv[2];
        else alg = NULL;

        ojr_generator *g = ojr_open(alg);
        ojr_system_seed(g);
        uint32_t buf[100];

        fprintf(stderr, "Testing algorithm \"%s\".\n", alg);
        while (1) {
            for (int i = 0; i < 100; ++i) {
                buf[i] = ojr_next32(g);
            }
            write(STDOUT_FILENO, buf, sizeof(buf));
        }
        /* NEVER EXITS */
    }
    f = balance_set();
    printf("rand() balance tests %sed.\n", f ? "fail" : "pass");
    if (f) {
        printf("Error code: %d\n", f);
        return EXIT_FAILURE;
    }
    /* TODO: Gaussians */

    return EXIT_SUCCESS;
}
