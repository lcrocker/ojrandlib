/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * This is the main source for the C library.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

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

/* External interface to agorithm list. Each algorithm has an integer
 * ID from 1 to <count> used to fetch info.
 */

static int _ojr_algorithm_count = 0;

extern int ojr_algorithm_count(void) {
    if (0 == _ojr_algorithm_count) {
        ojr_algorithm **aap = _ojr_algorithms;
        while (NULL != *aap) { ++aap; ++_ojr_algorithm_count; }
    }
    return _ojr_algorithm_count;
}

extern int ojr_algorithm_id(const char *name) {
    int i, c = ojr_algorithm_count();
    for (i = 0; i < c; ++i) {
        if (0 == STRCASECMP(name, (_ojr_algorithms[i])->name)) {
            return i + 1;
        }
    }
    return 0;
}

extern char *ojr_algorithm_name(int id) {
    if (id < 1 || id > ojr_algorithm_count()) return NULL;
    return (_ojr_algorithms[id - 1])->name;
}

extern int ojr_algorithm_seedsize(int id) {
    if (id < 1 || id > ojr_algorithm_count()) return 0;
    return (_ojr_algorithms[id - 1])->seedsize;
}

extern int ojr_algorithm_statesize(int id) {
    if (id < 1 || id > ojr_algorithm_count()) return 0;
    return (_ojr_algorithms[id - 1])->statesize;
}

extern int ojr_algorithm_bufsize(int id) {
    if (id < 1 || id > ojr_algorithm_count()) return 0;
    return (_ojr_algorithms[id - 1])->bufsize;
}

/* Put the given new generator structure into a valid state
 * (but without any allocated buffers).
 */

void _ojr_init(ojr_generator *g) {
    assert(0 != g);
    memset(g, 0, sizeof(ojr_generator));
    g->_status = 0xb1e55ed0;
}

/* Getters and setters typically used only by language bindings. None of
 * these allocate memory for the arrays passed in, nor do they free memory,
 * so don't use them in client code!
 */

int _ojr_structure_size(void) { return sizeof(ojr_generator); }

int _ojr_get_status(ojr_generator *g) { return g->_status; }
void _ojr_set_status(ojr_generator *g, int s) { g->_status = s; }

int _ojr_get_seedsize(ojr_generator *g) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    return g->seedsize;
}

uint32_t *_ojr_get_seed(ojr_generator *g) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    return g->seed;
}

int _ojr_get_statesize(ojr_generator *g) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    return g->statesize;
}

uint32_t *_ojr_get_state(ojr_generator *g) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    return g->state;
}

int _ojr_get_bufsize(ojr_generator *g) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    return g->bufsize;
}

uint32_t *_ojr_get_buffer(ojr_generator *g) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    return g->buf;
}

uint32_t *_ojr_get_buffer_ptr(ojr_generator *g) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    return g->bptr;
}

int _ojr_get_algorithm(ojr_generator *g) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    return g->algorithm;
}

void _ojr_set_seed(ojr_generator *g, uint32_t *seed, int size) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    g->seedsize = size;
    g->seed = seed;
}

void _ojr_set_state(ojr_generator *g, uint32_t *state, int size) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    g->statesize = size;
    g->state = state;
}

void _ojr_set_buffer(ojr_generator *g, uint32_t *buf, int size) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    g->bufsize = size;
    g->buf = g->bptr = buf;
}

void _ojr_set_buffer_ptr(ojr_generator *g, uint32_t *bptr) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    assert(0 != bptr && (bptr - g->buf) <= g->bufsize);
    g->bptr = bptr;
}

void _ojr_set_algorithm(ojr_generator *g, int id) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    assert(id > 0 && id <= ojr_algorithm_count());
    g->algorithm = id;
    g->ap = _ojr_algorithms[id - 1];
}

void _ojr_call_open(ojr_generator *g) {
    if (NULL != g->ap->open) (*g->ap->open)(g);
}

void _ojr_call_close(ojr_generator *g) {
    if (NULL != g->ap->close) (*g->ap->close)(g);
}

void _ojr_call_seed(ojr_generator *g) {
    assert(g->ap->seed);
    (*g->ap->seed)(g);
}

void _ojr_call_reseed(ojr_generator *g, int val) {
    if (NULL != g->ap->reseed) (*g->ap->reseed)(g, val);
    else ojr_default_reseed(g, val);
}

void _ojr_call_refill(ojr_generator *g) {
    assert(g->ap->refill);
    (*g->ap->refill)(g);
}

/* If the algorithm has no need for a special reseed function, it can call
 * this default one. This is not intended to be a user function.
 */

void ojr_default_reseed(ojr_generator *g, int value) {
    int i, mask = 0x11111111;

    if (g->statesize < 4) g->state[0] ^= value;
    else {
        for (i = 0; i < 4; ++i) {
            g->state[i] ^= (value & mask);
            mask <<= 1;
        }
    }
}

/* General-purpose seed function--not intended for users; this will be
 * used by algorithms as a starting point.
 */

void ojr_default_seed(ojr_generator *g) {
    int i, c, x, size = g->seedsize;
    uint32_t *sp = g->seed;

    if (size < g->statesize) {
        x = 232497429;
        for (i = 0; i < g->statesize; ++i) {
            x = (69069 * x) + 764385;
            g->state[i] = x;
        }
        for (i = 0; i < size; ++i) {
            g->state[i] ^= *sp++;
        }
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

int ojr_get_seedsize(ojr_generator *g) { return g->seedsize; }

/* Return the stored seed. If we provided enough space for the whole seed,
 * and the generator was only seeded once, then return 1, indicating that
 * the seed we got can be used to reproduce the sequence. Otherwise,
 * return 0.
 */

int ojr_get_seed(ojr_generator *g, uint32_t *seed, int size) {
    int c;
    assert(0xb1e55ed2 == (g->_status & 0xfffffff2));

    c = (size < g->seedsize) ? size : g->seedsize;
    memmove(seed, g->seed, 4 * c);

    if (size >= g->seedsize && 0xb1e55ed2 == g->_status) return 1;
    return 0;
}

/* Which algorithm are we using?
 */

int ojr_get_algorithm(ojr_generator *g) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    return g->algorithm;
}

void *ojr_get_extra(ojr_generator *g) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    return g->extra;
}

void ojr_set_extra(ojr_generator *g, void *extra) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    g->extra = extra;
}

/* Return next 32, 16, or 64 random bits from buffer. Buffer is typed as
 * array of 32-bit words, so that's the default on which the others are based,
 */

uint32_t ojr_next32(ojr_generator *g) {
    assert(0xb1e55ed2 == (g->_status & 0xfffffff2));

    if (g->bptr == g->buf) {
        _ojr_call_refill(g);
        g->bptr = g->buf + g->bufsize;
    }
    return *--g->bptr;
}

uint16_t ojr_next16(ojr_generator *g) {
    uint16_t r16;
    uint32_t r32;

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
    uint64_t r64;

    r64 = ojr_next64(g);
    ieee.i = (r64 >> 12) | 0x3FF0000000000000;
    return ieee.d - 1.0;
}

/* Return double in range (-1,1).
 */
double ojr_next_signed_double(ojr_generator *g) {
    int sign;
    uint64_t r64;

    do {
        r64 = ojr_next64(g);
        sign = (int)r64 & 1;
        r64 >>= 12;
    } while (sign && 0LL == r64);
    ieee.i = r64 | 0x3FF0000000000000;
    ieee.d -= 1.0;
    return sign ? -ieee.d : ieee.d;
}

/* Return a well-balanced random integer from 0 to limit-1. Limited to 16 bits
 * for performance, because this kind of function is generally used to select
 * from among a smaller sample of things, like cards in a deck.
 */

int ojr_rand(ojr_generator *g, int limit) {
    int v, m = limit - 1;
    assert(0xb1e55ed2 == (g->_status & 0xfffffff2));
    assert(limit > 0 && limit <= 0xFFFF);

    m |= m >> 1;
    m |= m >> 2;
    m |= m >> 4;
    m |= m >> 8;
    /* m |= m >> 16; Uncomment this if you change to 32 bits */
    /* m |= m >> 32; Uncomment this if you change to 64 bits */

    do {
        v = ojr_next16(g) & m;
    } while (v >= limit);
    return v;
}

/* Skip over <count> values of the generator without returning them.
 */
void ojr_discard(ojr_generator *g, int count) {
    int inbuf = g->bptr - g->buf;

    g->_leftover = 0;
    if (count <= inbuf) {
        g->bptr -= count;
        return;
    }
    count -= inbuf;
    g->bptr = g->buf + g->bufsize;

    do {
        _ojr_call_refill(g);
        if (count <= g->bufsize) {
            g->bptr -= count;
            count = 0;
        } else {
            count -= g->bufsize;
        }
    } while (count);
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

    if (size < 2) return;
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

    if (size < 2) return;
    if (count == size) --count;

    for (i = 0; i < count; ++i) {
        r = ojr_rand(g, size - i);
        SWAP(i, i + r);
    }
}
