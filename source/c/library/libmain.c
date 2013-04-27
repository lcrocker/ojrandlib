/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * This is the main source for the C library.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "ojrandlib.h"

extern void _ojr_jkiss_seed(ojr_generator *g);
extern void _ojr_jkiss_reseed(ojr_generator *g, uint32_t *seed, int size);
extern void _ojr_jkiss_refill(ojr_generator *g);

ojr_algorithm _ojr_algorithm_jkiss = {
    "jkiss", 4, 4, 256,
    _ojr_jkiss_seed,
    _ojr_jkiss_reseed,
    _ojr_jkiss_refill,
};

ojr_algorithm *_ojr_algorithms[] = {
    &_ojr_algorithm_jkiss,
    NULL,
};

/* How many pre-loaded algorithms are available?
 */

int ojr_algorithm_count(void) {
    int c = 0;
    ojr_algorithm **aap = _ojr_algorithms;

    while (NULL != *aap) { ++aap; ++c; }
    return c;
}

/* Give me a list of the available algorithms.
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
            if (0 == strcmp(aname, (*aap)->name)) break;
            ++aap;
        }
        if (! *aap) return NULL;
    }
    ap = *aap;
    gp = malloc(sizeof(ojr_generator));
    if (gp) {
        gp->algorithm = ap;
        gp->seedsize = ap->seedsize;
        gp->statesize = ap->statesize;
        gp->bufsize = ap->bufsize;

        gp->seed = malloc(4 * gp->seedsize);
        if (gp->seed) {
            gp->state = malloc(4 * gp->statesize);
            if (gp->state) {
                gp->buf = malloc(4 * gp->bufsize + 6);
                if (gp->buf) {
                    gp->_status = 0x5eed1e55;
                    gp->bptr = gp->buf; /* Empty */

                    return gp;
                }
                free(gp->state);
            }
            free(gp->seed);
        }
        free(gp);
    }
    return NULL;
}

/* We're done with this generator. Free up allocated memory.
 */

void ojr_close(ojr_generator *g) {
    assert(g->seed && g->state && g->buf);
    free(g->buf);
    free(g->state);
    free(g->seed);
    free(g);
}

/* Seed the generator with an array of passed-in uints. If we are passed NULL,
 * gather system entropy and use that.
 */

void ojr_seed(ojr_generator *g, uint32_t *seed, int size) {
    int i, c;
    uint32_t *sp;
    assert(0xca11ab1e == g->_status || 0x5eed1e55 == g->_status);

    if (NULL == seed) {
        ojr_get_system_entropy(g->seed, g->seedsize);
    } else if (size < g->seedsize) {
        for (i = 0; i < g->seedsize; ++i) g->seed[i] = i + 1;
        for (i = 0; i < size; ++i) g->seed[i] ^= seed[i];
    } else {
        memmove(g->seed, seed, 4 * g->seedsize);
        size -= g->seedsize;
        sp = seed + size;

        while (size) {
            c = (size < g->seedsize) ? size : g->seedsize;
            for (i = 0; i < c; ++i) g->seed[i] ^= *sp++;
            size -= c;
        }
    }
    (g->algorithm->seed)(g);
    g->bptr = g->buf; /* Empty */
    g->_status = 0xca11ab1e;
    return;
}

/* Apply new seed on top of existing one.
 */

void ojr_reseed(ojr_generator *g, uint32_t *seed, int size) {
    assert(0xca11ab1e == g->_status);
    (g->algorithm->reseed)(g, seed, size);
    return;
}

/* If the algorithm has no need for a special reseed function, it can call
 * this default one. This is not intended to be a user function.
 */

void _ojr_default_reseed(ojr_generator *g, uint32_t *seed, int size) {
    int i, c;

    while (size) {
        c = (size <= g->statesize) ? size : g->statesize;
        for (i = 0; i < c; ++i) g->state[i] ^= seed[i];
        size -= c;
    }
}

/* Most algorithms will just copy the seed into the state vector, and perhaps
 * munge it a bit afterward. This is a utility function to help that. It is not
 * intended to be a user function.
 */

void _ojr_default_seed(ojr_generator *g) {
    memset(g->state, 0, 4 * g->statesize);
    _ojr_default_reseed(g, g->seed, g->seedsize);
}

/* Return the stored seed, which can be used to reproduce the same sequence
 * (assuming the passed buffer is large enough to contain the full seed).
 * Return the number of words actually copied, which will be the lesser of
 * the full seed size and the size of the passed buffer.
 */

int get_seed(ojr_generator *g, uint32_t *seed, int size) {
    int c;
    assert(0x5eed1e55 == g->_status || 0xca11ab1e == g->_status);


    c = (size < g->seedsize) ? size : g->seedsize;
    memmove(seed, g->seed, 4 * c);
    return c;
}

/* Return next 16, 32, or 64 random bits from buffer. Buffer is typed as
 * array of 16-bit shorts, so that's out minimum alignment unit. Refill
 * buffer from algorithm when needed. Unaligned leftover bytes are moved to
 * after the buffer end--that's why it was allocated with 6 extra bytes.
 */

uint16_t ojr_next16(ojr_generator *g) {
    assert(0xca11ab1e == g->_status);

    if (g->bptr == g->buf) {
        (*g->algorithm->refill)(g);
        g->bptr = g->buf + (2 * g->bufsize);
    }
    return *--g->bptr;
}

uint32_t ojr_next32(ojr_generator *g) {
    int r = (g->bptr - g->buf);
    uint16_t left;
    assert(0xca11ab1e == g->_status);

    if (r < 2) {
        g->buf[g->bufsize] = *g->buf;
        (*g->algorithm->refill)(g);
        g->bptr = g->buf + 2 * g->bufsize + r;
    }
    g->bptr -= 2;
    return *((uint32_t *)(g->bptr));
}

uint64_t ojr_next64(ojr_generator *g) {
    int r = (g->bptr - g->buf);
    assert(0xca11ab1e == g->_status);

    if (r < 4) {
        memmove(g->buf + g->bufsize, g->buf, 6);
        (*g->algorithm->refill)(g);
        g->bptr = g->buf + 2 * g->bufsize + r;
    }
    g->bptr -= 4;
    return *((uint64_t *)(g->bptr));
}

/* For those of you into floating point, return one in [0,1).
 * Assumes ieee-64 floating point format.
 */
static union {
    double d;
    uint64_t i;
} ieee;

double ojr_next_double(ojr_generator *g) {
    uint64_t r = ojr_next64(g);
    assert(0xca11ab1e == g->_status);

    ieee.i = (r >> 12) | 0x3FF0000000000000;
    return ieee.d - 1.0;
}

/* Return normally-distributed double using modified Ziggurat algorithm.
 */

double ojr_next_gaussian(ojr_generator *g) {
    /* TODO */
    return 0.0;
}

/* Return a well-balanced random integer from 0 to limit-1.
 */
int ojr_rand(ojr_generator *g, int limit) {
    int v, m = limit - 1;
    assert(0xca11ab1e == g->_status);
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
    assert(0xca11ab1e == g->_status);

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
    assert(0xca11ab1e == g->_status);

    if (count < 2) return;
    if (count == size) --count;

    for (i = 0; i < count; ++i) {
        r = ojr_rand(g, size - i);
        SWAP(i, i + r);
    }
}
