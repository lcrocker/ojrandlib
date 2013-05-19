/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Statistical tests for RNGs. If run with "-d <algorithm>" it will send
 * bits to stdout from that algorithm, which can be piped into dieharder.
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#include "ojrandlib.h"

#define ACOUNT 3
#define CHOOSE(a) (a)[ojr_rand(NULL,sizeof(a)/sizeof((a)[0]))]

static long iterations = 2000000;

typedef struct _counter {
    int n;
    long total, clipped;
    double mn, mx, rw;
    char *test, *alg;
    long *counts;
    double *ev;
} counter;

static counter *newcounter(int size) {
    counter *c = calloc(1, sizeof(counter));
    c->n = size;
    c->counts = calloc(c->n, sizeof(long));
    return c;
}

static void closecounter(counter *c) {
    if (c->ev) free(c->ev);
    if (c->counts) free(c->counts);
    free(c);
}

static void setrange(counter *c, double mn, double mx) {
    c->mn = mn;
    c->mx = mx;
    // Precalculate reciprocal of bucket width
    c->rw = c->n / (mx - mn);
}

#define INC(c,b) do { ++(c)->total; \
if ((b) < 0 || (b) >= (c)->n) ++(c)->clipped; \
else ++(c)->counts[b]; } while (0)

#define INCV(c,v) do { ++(c)->total; \
if ((v) <= (c)->mn || (v) >= (c)->mx) ++(c)->clipped; \
else ++(c)->counts[(int)(((v) - (c)->mn) * (c)->rw)]; } while (0)

#define PI 3.1415926535897932385
#define LNSQ2PI 0.9189385332046727418
#define ISQ2 0.70710678118654752444

static double lanczos[9] = {
    0.999999999999809932, 676.520368121885099, -1259.13921672240287,
    771.323428777653079, -176.615029162140599, 12.5073432786869048,
    -0.138571095265720117, 9.98436957801957086e-6, 1.50563273514931156e-7
};

static double lngamma(double z) {
    if (z < 0.5) return log(PI / sin(PI * z)) - lngamma(1.0 - z);

    z -= 1.0;
    double base = z + 7.5;
    double sum = 0;

    for (int j = 8; j > 0; --j) { sum += lanczos[j] / (z + (double)j); }
    sum += lanczos[0];
    return ((LNSQ2PI + log(sum)) - base) + log(base) * (z + 0.5);
}

#define PQITERATIONS 120
#define PQEPSILON 3e-7

static double pseries(double a, double x) {
    double ap = a;
    double sum = 1.0 / a;
    double del = sum;
    double lng = lngamma(a);

    for (int i = 0; i < PQITERATIONS; ++i) {
        ap += 1.0;
        del *= x / ap;
        sum += del;
        if (fabs(del) < fabs(sum) * PQEPSILON) {
            return sum * exp(-x + a * log(x) - lng);
        }
    }
    // Iterations maxed
    return sum * exp(-x + a * log(x) - lng);
}

static double qcfrac(double a, double x) {
    double g, gold = 0.0, fac = 1.0, b1 = 1.0;
    double anf, ana, an, a1, b0 = 0.0, a0 = 1.0;
    double lng = lngamma(a);

    a1 = x;
    for (int i = 1; i <= PQITERATIONS; ++i) {
        an = (double)i;
        ana = an - a;
        a0 = (a1 + a0 * ana) * fac;
        b0 = (b1 + b0 * ana) * fac;

        anf = an * fac;
        a1 = x * a0 + anf * a1;
        b1 = x * b0 + anf * b1;

        if (a1) {
            fac = 1.0 / a1;
            g = b1 * fac;
            if (fabs((g - gold) / g) < PQEPSILON) {
                return g * exp(-x + a * log(x) - lng);
            }
            gold = g;
        }
    }
    // Iterations maxed
    return g * exp(-x + a * log(x) - lng);
}

static double igfp(double a, double x) {
    assert(a > 0.0 && x >= 0.0);
    if (x < a + 1.0) return pseries(a, x);
    else return 1.0 - qcfrac(a, x);
}

static double igfq(double a, double x) {
    assert(a > 0.0 && x >= 0.0);
    if (x < a + 1.0) return 1.0 - pseries(a, x);
    else return qcfrac(a, x);
}

static double normcdf(double z) {
    z *= ISQ2;
    if (z < 0.0) return 0.5 * igfq(0.5, z * z);
    else return 0.5 * (1.0 + igfp(0.5, z * z));
}

static double pvalue(counter *c) {
    double d, d2, m, ev, t = 0.0, chi2 = 0.0;
    m = ev = (double)(c->total) / c->n;

    for (int i = 0; i < c->n; ++i) {
        if (c->ev) ev = c->ev[i];
        d = (double)(c->counts[i]) - ev;
        d2 = d * d;
        t += d2;
        chi2 += d2 / ev;
    }
    return igfq(0.5 * (c->n - 1), 0.5 * chi2);
}

static double results(counter *c) {
    double pv = pvalue(c);
    printf("%10s%10s   n = %4d   p-value = %6.3f\n",
        c->alg, c->test, c->n, pv);
    if (pv < 0.0001) {
        printf("*** TEST FAILURE ***\n");
    }
    return pv;
}

static int bsizes[] = { 7, 32, 52, 53, 65, 256, 1000 };
static char *testnames[] = {
    "int.uni", "flt.uni", "sgn.uni", "sgn.nrm", "flt.exp"
};

static int distribution_test(ojr_generator *g, int type) {
    int r, n;
    double d, bw, left, right;

    n = CHOOSE(bsizes);
    counter *c = newcounter(n);

    if (1 == type) setrange(c, 0.0, 1.0);
    else if (2 == type) setrange(c, -1.0, 1.0);
    else if (3 == type) setrange(c, -3.0, 3.0);
    else if (4 == type) setrange(c, 0.0, 10.0);

    switch (type) {
    case 0:
        for (long i = 0; i < iterations; ++i) {
            r = ojr_rand(g, n);
            INC(c, r);
        }
        break;
    case 1:
        for (long i = 0; i < iterations; ++i) {
            d = ojr_next_double(g);
            INCV(c, d);
        }
        break;
    case 2:
        for (long i = 0; i < iterations; ++i) {
            d = ojr_next_signed_double(g);
            INCV(c, d);
        }
        break;
    case 3:
        for (long i = 0; i < iterations; ++i) {
            d = ojr_next_normal(g);
            INCV(c, d);
        }
        break;
    case 4:
        for (long i = 0; i < iterations; ++i) {
            d = ojr_next_exponential(g);
            INCV(c, d);
        }
        break;
    }
    c->alg = ojr_algorithm_name(g->algorithm);
    c->test = testnames[type];

    if (type > 2) {
        assert(NULL == c->ev);
        c->ev = calloc(c->n, sizeof(double));
        bw = (c->mx - c->mn) / c->n;        
    }
    if (3 == type) {
        left = normcdf(c->mn);
        for (int i = 0; i < (c->n + 1) >> 1; ++i) {
            right = normcdf(c->mn + (i + 1) * bw);
            c->ev[i] = c->ev[(c->n - 1) - i] = c->total * (right - left);
            left = right;
        }
    } else if (4 == type) {
        assert(0.0 == c->mn && 10.0 == c->mx);
        left = 0.0;
        for (int i = 0; i < c->n; ++i) {
            right = 1.0 - exp(-bw * (i + 1));
            c->ev[i] = c->total * (right - left);
            left = right;
        }
    }
    double pv = results(c);
    closecounter(c);
    return (pv < 0.001);
}

static ojr_generator *gens[ACOUNT];

int loop(int count) {
    int f = 0;
    for (int i = 0; i < ACOUNT; ++i) {
        gens[i] = ojr_open(ojr_algorithm_name(i+1));
        ojr_system_seed(gens[i]);
    }

    for (int i = 0; i < count; ++i) {
        ojr_generator *g = CHOOSE(gens);
        int t = ojr_rand(NULL, 100);

        if (t < 30) {
            f = distribution_test(g, 0);
        } else if (t < 50) {
            f = distribution_test(g, 1);
        } else if (t < 70) {
            f = distribution_test(g, 2);            
        } else if (t < 85) {
            f = distribution_test(g, 3);            
        } else if (t < 100) {
            f = distribution_test(g, 4);            
        }
        if (f) break;
    }
    for (int i = 0; i < ACOUNT; ++i) ojr_close(gens[i]);
    return f;
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
    f = loop(100);
    return EXIT_SUCCESS;
}
