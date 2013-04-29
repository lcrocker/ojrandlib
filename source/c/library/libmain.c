/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * The included Gaussian function is based on ZIGNOR by Jurgen A. Doornik:
 * "An Improved Ziggurat Method to Generate Normal Random Samples", mimeo,
 * Nuffield College, University of Oxford <www.doornik.com/research>.
 * This notice should be maintained in modified versions of the code.
 * No warranty is given regarding the correctness of this code. 
 *
 * This is the main source for the C library.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "ojrandlib.h"

/* This is the global list of available algorithms. Each algorithm source file
 * must contain a structure whose name is public, and these are listed here.
 */

extern ojr_algorithm ojr_algorithm_jkiss;
extern ojr_algorithm ojr_algorithm_mt19937;

ojr_algorithm *_ojr_algorithms[] = {
    &ojr_algorithm_jkiss,
    &ojr_algorithm_mt19937,
    NULL,
};

/* Get the address of the info structure for an algorithm by name.
 */

ojr_algorithm *ojr_algorithm_info(char *name) {
    ojr_algorithm **aap = _ojr_algorithms;
    while (NULL != *aap) {
        if (0 == STRCASECMP(name, (*aap)->name)) return *aap;
        ++aap;
    }
    return NULL;
}

/* How many pre-loaded algorithms are available?
 */

int ojr_algorithm_count(void) {
    int c = 0;
    ojr_algorithm **aap = _ojr_algorithms;
    while (NULL != *aap) { ++aap; ++c; }
    return c;
}

/* Give me a pointer to the list of the available algorithms.
 */

ojr_algorithm **ojr_algorithm_list(void) {
    return _ojr_algorithms;
}

/* Give me a new generator given an algorithm name. If name is NULL,
 * just give us the first on on the list. Generator won't be usable
 * until seeded. Return NULL if something goes wrong.
 */

ojr_generator *ojr_new(char *aname) {
    ojr_algorithm *ap, **aap = _ojr_algorithms;
    ojr_generator *gp = NULL;

    if (NULL != aname) {
        while (NULL != *aap) {
            if (0 == STRCASECMP(aname, (*aap)->name)) break;
            ++aap;
        }
        if (! *aap) return NULL;
    }
    ap = *aap;
    gp = malloc(sizeof(ojr_generator));
    if (gp) {
        gp->algorithm = ap;
        gp->_leftover = gp->seedsize = 0;
        gp->statesize = ap->statesize;
        gp->bufsize = ap->bufsize;
        gp->seed = gp->extra = NULL;

        gp->state = malloc(4 * gp->statesize);
        if (gp->state) {
            gp->buf = malloc(4 * gp->bufsize);
            if (gp->buf) {
                gp->_status = 0x5eed1e55;
                gp->bptr = gp->buf; /* Empty */

                (*ap->open)(gp);
                return gp;
            }
            free(gp->state);
        }
        free(gp);
    }
    return NULL;
}

/* We're done with this generator. Free up allocated memory.
 */

void ojr_close(ojr_generator *g) {
    assert(g->state && g->buf);
    assert(0x5eed1e55 == g->_status || 0x5eeded01 == g->_status
        || 0x5eeded02 == g->_status);

    (*g->algorithm->close)(g);
    if (g->seed) free(g->seed);
    free(g->buf);
    free(g->state);
    free(g);
}

/* Common code for seed & reseed. Create the seed buffer in the generator
 * object, fill it from the passed-in array (or system entropy if we gave
 * a NULL pointer).
 */

static int _ojr_copy_seed(ojr_generator *g, uint32_t *seed, int size) {
    if (g->seed) { free(g->seed); }

    if (NULL == seed) {
        if (0 == size) { size = g->algorithm->seedsize; }

        g->seed = malloc(4 * size);
        if (! g->seed) return 0;
        g->seedsize = size;
        ojr_get_system_entropy(g->seed, g->seedsize);
    } else {
        g->seed = malloc(4 * size);
        if (! g->seed) return 0;
        g->seedsize = size;
        memmove(g->seed, seed, 4 * size);
    }
    return g->seedsize;
}

/* Seed the generator from passed-in array.
 */

int ojr_seed(ojr_generator *g, uint32_t *seed, int size) {
    int r;
    assert(0x5eed1e55 == g->_status || 0x5eeded01 == g->_status
        || 0x5eeded02 == g->_status);

    r = _ojr_copy_seed(g, seed, size);
    if (r) {
        (*g->algorithm->seed)(g);
        g->bptr = g->buf; /* Empty */

        if (0x5eed1e55 == g->_status) g->_status = 0x5eeded01;
        else g->_status = 0x5eeded02;
    }
    return r;
}

/* General-purpose seed function--not intended for users; this will be
 * used by algorithms as a starting point.
 */

void _ojr_default_seed(ojr_generator *g) {
    int i, c, size = g->seedsize;
    uint32_t *sp = g->seed;

    if (size < g->statesize) {
        for (i = 0; i < g->statesize; ++i) g->state[i] = i + 1;
        for (i = 0; i < size; ++i) g->state[i] ^= *sp++;
    } else {
        memmove(g->state, g->seed, 4 * g->statesize);
        size -= g->statesize;

        while (size) {
            c = (size <= g->statesize) ? size : g->statesize;
            for (i = 0; i < c; ++i) g->state[i] ^= *sp++;
            size -= c;
        }
    }
}

/* Apply new seed on top of existing one.
 */

int ojr_reseed(ojr_generator *g, uint32_t *seed, int size) {
    int r;
    assert(0x5eeded01 == g->_status || 0x5eeded02 == g->_status);

    r = _ojr_copy_seed(g, seed, size);
    if (r) {
        (*g->algorithm->reseed)(g);
        g->_status = 0x5eeded02;
    }
    return r;
}

/* If the algorithm has no need for a special reseed function, it can call
 * this default one. This is not intended to be a user function.
 */

void _ojr_default_reseed(ojr_generator *g) {
    int i, c, size = g->seedsize;
    uint32_t *sp = g->seed;

    if (size < g->statesize) {
        for (i = 0; i < size; ++i) g->state[i] ^= *sp++;
    } else {
        while (size) {
            c = (size <= g->statesize) ? size : g->statesize;
            for (i = 0; i < c; ++i) g->state[i] ^= *sp++;
            size -= c;
        }
    }
}

/* Return the stored seed. If we provided enough space for the whole seed,
 * and the generator was only seeded once, then return 1, indicating that
 * the seed we got can be used to reproduce the sequence. Otherwise,
 * return 0.
 */

int ojr_get_seed(ojr_generator *g, uint32_t *seed, int size) {
    int c;
    assert(0x5eeded01 == g->_status || 0x5eeded02 == g->_status);

    c = (size < g->seedsize) ? size : g->seedsize;
    memmove(seed, g->seed, 4 * c);

    if (size >= g->seedsize && 0x5eeded01 == g->_status) return 1;
    return 0;
}

/* Return next 32, 16, or 64 random bits from buffer. Buffer is typed as
 * array of 16-bit shorts, so that's out minimum alignment unit. Refill
 * buffer from algorithm when needed. Unaligned leftover bytes are moved to
 * after the buffer end--that's why it was allocated with 6 extra bytes.
 */

uint32_t ojr_next32(ojr_generator *g) {
    assert(0x5eeded01 == g->_status || 0x5eeded02 == g->_status);

    if (g->bptr == g->buf) {
        (*g->algorithm->refill)(g);
        g->bptr = g->buf + g->bufsize;
    }
    return *--g->bptr;
}

uint16_t ojr_next16(ojr_generator *g) {
    uint16_t r16;
    uint32_t r32;
    assert(0x5eeded01 == g->_status || 0x5eeded02 == g->_status);

    if (g->_leftover) {
        r16 = g->_leftover & 0xFFFF;
        g->_leftover = 0;
        return r16;
    }
    r32 = ojr_next32(g);
    g->_leftover = (r32 >> 16) | 0x1ef70000;
    return r32 & 0xFFFF;
}

uint64_t ojr_next64(ojr_generator *g) {
    uint64_t r64;
    assert(0x5eeded01 == g->_status || 0x5eeded02 == g->_status);

    r64 = (uint64_t)ojr_next32(g);
    return (r64 << 32) | ojr_next32(g);
}

/* Return double in range [0,1).
 * Assumes ieee-64 floating point format.
 */
static union {
    double d;
    uint64_t i;
} ieee;

double ojr_next_double(ojr_generator *g) {
    uint64_t r64 = ojr_next64(g);
    assert(0x5eeded01 == g->_status || 0x5eeded02 == g->_status);

    ieee.i = (r64 >> 12) | 0x3FF0000000000000;
    return ieee.d - 1.0;
}

/* Return double in range (-1,1).
 */
double ojr_next_signed_double(ojr_generator *g) {
    int sign;
    uint64_t r64;
    assert(0x5eeded01 == g->_status || 0x5eeded02 == g->_status);

    do {
        r64 = ojr_next64(g);
        sign = (int)r64 & 1;

        ieee.i = (r64 >> 12) | 0x3FF0000000000000;
        ieee.d -= 1.0;
    } while (sign && (0.0 == ieee.d));
    return sign ? -ieee.d : ieee.d;
}

/* Beginning of ZIGNOR section. See paper for details.
 */

#define ZIGNOR_C 128
#define ZIGNOR_R 3.442619855899
#define ZIGNOR_V 9.91256303526217e-3

static double s_adZigX[ZIGNOR_C + 1], s_adZigR[ZIGNOR_C];
static int _zignor_table_initialized = 0;

static void _zignor_init(int iC, double dR, double dV) {
    int i;
    double f;

    f = exp(-0.5 * dR * dR);
    s_adZigX[0] = dV / f; /* [0] is bottom block: V / f(R) */
    s_adZigX[1] = dR;
    s_adZigX[iC] = 0;

    for (i = 2; i < iC; ++i) {
        s_adZigX[i] = sqrt(-2 * log(dV / s_adZigX[i - 1] + f));
        f = exp(-0.5 * s_adZigX[i] * s_adZigX[i]);
    }
    for (i = 0; i < iC; ++i) {
        s_adZigR[i] = s_adZigX[i + 1] / s_adZigX[i];
    }
}

static double _normal_tail(ojr_generator *g, double dmin, int neg) {
    double x, y;

    do {
        x = log(ojr_next_double(g)) / dmin;
        y = log(ojr_next_double(g));
    } while (-2 * y < x * x);
    return neg ? x - dmin : dmin - x;
}

double ojr_next_gaussian(ojr_generator *g) {
    int i, sign;
    double x, a, u, f0, f1;
    uint64_t r64;
    assert(0x5eeded01 == g->_status || 0x5eeded02 == g->_status);

    if (! _zignor_table_initialized) {
        _zignor_init(ZIGNOR_C, ZIGNOR_R, ZIGNOR_V);
        _zignor_table_initialized = 1;
    }
    while (1) {
        do {
            r64 = ojr_next64(g);
            sign = (int)r64 & 1;

            ieee.i = (r64 >> 12) | 0x3FF0000000000000;
            ieee.d -= 1.0;
        } while (sign && (0.0 == ieee.d));

        a = ieee.d;
        u = sign ? -a : a;
        i = ((int)r64 >> 1) & 0x7F;

        if (a < s_adZigR[i]) return u * s_adZigX[i];

        if (0 == i) return _normal_tail(g, ZIGNOR_R, sign);

        x = u * s_adZigX[i];
        f0 = exp( -0.5 * (s_adZigX[i] * s_adZigX[i] - x * x) );
        f1 = exp( -0.5 * (s_adZigX[i + 1] * s_adZigX[i + 1] - x * x) );

        if (f1 + ojr_next_double(g) * (f0 - f1) < 1.0) return x;
    }
}

/* Return a well-balanced random integer from 0 to limit-1.
 */

int ojr_rand(ojr_generator *g, int limit) {
    int v, m = limit - 1;
    assert(0x5eeded01 == g->_status || 0x5eeded02 == g->_status);
    assert(limit > 0);

    m |= m >> 1;
    m |= m >> 2;
    m |= m >> 4;
    m |= m >> 8;
    m |= m >> 16;

    do {
        v = ojr_next32(g) & m;
    } while (v >= limit);
    return v;
}

#define SWAP(a,b) do { if ((a) != (b)) { \
t = array[a]; array[a] = array[b]; array[b] = t; \
} } while(0)

/* Move to the top of the array a randomly-chosen combination of <count>
 * elements, where each combination and permutation is equally likely.
 * If <count> == <size>, this becomes a standard Fisher-Yates shuffle.
 */

void ojr_shuffle_int_array(
ojr_generator *g, int *array, int size, int count) {
    int i, r, t;
    assert(0x5eeded01 == g->_status || 0x5eeded02 == g->_status);

    if (count < 2) return;
    if (count == size) --count;

    for (i = 0; i < count; ++i) {
        r = ojr_rand(g, size - i);
        SWAP(i, i + r);
    }
}

void ojr_shuffle_pointer_array(
ojr_generator *g, void **array, int size, int count) {
    int i, r;
    void *t;
    assert(0x5eeded01 == g->_status || 0x5eeded02 == g->_status);

    if (count < 2) return;
    if (count == size) --count;

    for (i = 0; i < count; ++i) {
        r = ojr_rand(g, size - i);
        SWAP(i, i + r);
    }
}
