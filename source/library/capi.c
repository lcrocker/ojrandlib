/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Basic C API functions.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "ojrandlib.h"


// Create a new generator
ojr_generator *ojr_open(const char *name) {
    int id = ojr_algorithm_id(name);
    int statesize = ojr_algorithm_statesize(id);
    int bufsize = ojr_algorithm_bufsize(id);
    ojr_generator *gp;
    uint32_t *sp, *bp;

    gp = malloc(sizeof(ojr_generator));
    if (gp) {
        sp = calloc(statesize, 4);
        if (sp) {
            bp = calloc(bufsize, 4);
            if (bp) {
                ojr_init(gp);
                gp->next = ojr_genlist_head;
                ojr_genlist_head = gp;

                ojr_set_algorithm(gp, id);
                ojr_set_state(gp, sp, statesize);
                ojr_set_buffer(gp, bp, bufsize);
                ojr_call_open(gp);
                return gp;
            }
            free(sp);
        }
        free(gp);
    }
    return NULL;
}

// We're done with this generator, free up allocated memory
void ojr_close(ojr_generator *g) {
    ojr_generator **p;
    assert(0x5eed1e55 == g->init);
    assert(g->state && g->buf);
    assert(NULL != ojr_genlist_head);

    ojr_call_close(g);

    p = &ojr_genlist_head;
    while (*p != g) p = &((*p)->next);
    *p = g->next;

    free(g->buf);
    free(g->state);
    free(g);
}

void ojr_array_seed(ojr_generator *g, uint32_t *seed, int size) {
    assert(0x5eed1e55 == g->init);
    g->bptr = g->buf;

    ojr_call_seed(g, seed, size);
    ojr_set_seeded(g, 1);
}

void ojr_int_seed(ojr_generator *g, int value) {
    ojr_array_seed(g, (uint32_t *)(&value), 1);
}

void ojr_system_seed(ojr_generator *g) {
    uint32_t *seed;
    int id = ojr_get_algorithm(g);
    int size = ojr_algorithm_seedsize(id);

    if (! (seed = malloc(4 * size))) return;
    ojr_get_system_entropy(seed, size);

    ojr_array_seed(g, seed, size);
    free(seed);
}

void ojr_network_seed(ojr_generator *g) {
    uint32_t *seed;
    int id = ojr_get_algorithm(g);
    int size = ojr_algorithm_seedsize(id);

    if (! (seed = malloc(4 * size))) return;
    ojr_get_random_org(seed, size);

    ojr_array_seed(g, seed, size);
    free(seed);
}

void ojr_reseed(ojr_generator *g, uint32_t *seed, int size) {
    assert(0x5eed1e55 == g->init);
    assert(ojr_get_seeded(g));
    ojr_call_reseed(g, seed, size);
}

/* Return next 32, 16, or 64 random bits from buffer. Buffer is typed as
 * array of 32-bit words, so that's the default on which the others are based.
 */

uint32_t ojr_next32(ojr_generator *g) {
    if (NULL == g) g = &ojr_default_generator;
    else { assert(0x5eed1e55 == g->init && (g->flags & OJRF_SEEDED)); }

    if (g->bptr == g->buf) {
        ojr_call_refill(g);
        g->bptr = g->buf + g->bufsize;
    }
    return *--g->bptr;
}

uint16_t ojr_next16(ojr_generator *g) {
    uint16_t r16;
    if (NULL == g) g = &ojr_default_generator;
    else { assert(0x5eed1e55 == g->init && (g->flags & OJRF_SEEDED)); }

    if (g->leftover) {
        r16 = g->leftover & 0xFFFF;
        g->leftover = 0;
        return r16;
    }
    if (g->bptr == g->buf) {
        ojr_call_refill(g);
        g->bptr = g->buf + g->bufsize;
    }
    --g->bptr;

    g->leftover = 0x10000 | *((uint16_t *)(g->bptr) + 1);
    return *((uint16_t *)(g->bptr));
}

uint64_t ojr_next64(ojr_generator *g) {
    uint64_t r = (uint64_t)ojr_next32(g);
    return (r << 32) | ojr_next32(g);
}

// Return double in range [0,1).
double ojr_next_double(ojr_generator *g) {
    uint64_t r = (ojr_next64(g) >> 12) | 0x3FF0000000000000;
    return *(double *)(&r) - 1.0;
}

// Return double in range (-1,1).
double ojr_next_signed_double(ojr_generator *g) {
    int sign;
    uint64_t r64;

    do {
        r64 = ojr_next64(g);
        sign = (int)r64 & 1;
        r64 >>= 12;
    } while (sign && 0LL == r64);

    r64 |= 0x3FF0000000000000;
    if (sign) return 1.0 - *(double *)(&r64);
    else return *(double *)(&r64) - 1.0;
}

// Functions next_normal and next_exponential are in ziggurat.c

/* Return a well-balanced random integer from 0 to limit-1. Limited to 16 bits
 * for performance, because this kind of function is generally used to select
 * from among a smaller sample of things, like cards in a deck.
 */
int ojr_rand(ojr_generator *g, int limit) {
    int v, m = limit - 1;
    assert(limit > 0 && limit <= 0x80000);

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

// Skip over <count> values of the generator without returning them.
void ojr_discard(ojr_generator *g, int count) {
    int inbuf = g->bptr - g->buf;
    if (NULL == g) g = &ojr_default_generator;
    else { assert(0x5eed1e55 == g->init); }

    g->leftover = 0;
    if (count <= inbuf) {
        g->bptr -= count;
        return;
    }
    count -= inbuf;
    g->bptr = g->buf + g->bufsize;

    do {
        ojr_call_refill(g);
        if (count <= g->bufsize) {
            g->bptr -= count;
            count = 0;
        } else {
            count -= g->bufsize;
        }
    } while (count);
}
