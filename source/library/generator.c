/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * This is the low-level access to generator structures.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "ojrandlib.h"


// Put the given new generator structure into a valid state
// (but without any allocated buffers).
void ojr_init(ojr_generator *g) {
    assert(0 != g);
    memset(g, 0, sizeof(ojr_generator));
    g->init = 0x5eed1e55;
}

/* Getters and setters typically used only by language bindings. None of
 * these allocate memory for the arrays passed in, nor do they free memory,
 * so don't use them in client code!
 */

int ojr_structure_size(void) { return sizeof(ojr_generator); }

int ojr_get_seeded(ojr_generator *g) {
    assert(0x5eed1e55 == g->init);
    return 0 != (g->flags & OJRF_SEEDED);
}

void ojr_set_seeded(ojr_generator *g, int val) {
    assert(0x5eed1e55 == g->init);
    if (val) g->flags |= OJRF_SEEDED;
    else g->flags &= ~OJRF_SEEDED;
}

int ojr_get_algorithm(ojr_generator *g) {
    assert(0x5eed1e55 == g->init);
    return g->algorithm;
}

void ojr_set_algorithm(ojr_generator *g, int id) {
    assert(0x5eed1e55 == g->init);
    assert(id > 0 && id <= ojr_algorithm_count());
    g->algorithm = id;
}

int ojr_get_statesize(ojr_generator *g) {
    assert(0x5eed1e55 == g->init);
    return g->statesize;
}

uint32_t *ojr_get_state(ojr_generator *g) {
    assert(0x5eed1e55 == g->init);
    return g->state;
}

void ojr_set_state(ojr_generator *g, uint32_t *state, int size) {
    assert(0x5eed1e55 == g->init);
    assert(0 != state && 0 != size);
    g->statesize = size;
    g->state = state;
}

int ojr_get_bufsize(ojr_generator *g) {
    assert(0x5eed1e55 == g->init);
    return g->bufsize;
}

uint32_t *ojr_get_buffer(ojr_generator *g) {
    assert(0x5eed1e55 == g->init);
    return g->buf;
}

uint32_t *ojr_get_buffer_ptr(ojr_generator *g) {
    assert(0x5eed1e55 == g->init);
    return g->bptr;
}

void ojr_set_buffer(ojr_generator *g, uint32_t *buf, int size) {
    assert(0x5eed1e55 == g->init);
    g->bufsize = size;
    g->buf = g->bptr = buf;
}

void ojr_set_buffer_ptr(ojr_generator *g, uint32_t *bptr) {
    assert(0x5eed1e55 == g->init);
    assert(0 != bptr && (bptr - g->buf) <= g->bufsize);
    g->bptr = bptr;
}

void *ojr_get_extra(ojr_generator *g) {
    assert(0x5eed1e55 == g->init);
    return g->extra;
}

void ojr_set_extra(ojr_generator *g, void *extra) {
    assert(0x5eed1e55 == g->init);
    g->extra = extra;
}

void ojr_call_open(ojr_generator *g) {
    int id = g->algorithm;
    void (*f)(ojr_generator *);

    if (0 == id) id = 1;
    f = ojr_algorithms[id - 1]->open;
    if (f) { (*f)(g); };
}

void ojr_call_close(ojr_generator *g) {
    int id = g->algorithm;
    void (*f)(ojr_generator *);

    if (0 == id) id = 1;
    f = ojr_algorithms[id - 1]->close;
    if (f) { (*f)(g); }
}

void ojr_call_seed(ojr_generator *g, uint32_t *seed, int size) {
    int id = g->algorithm;
    void (*f)(ojr_generator *, uint32_t *, int);

    if (0 == id) id = 1;
    f = ojr_algorithms[id - 1]->seed;
    if (f) { (*f)(g, seed, size); }
    else ojr_default_seed(g, seed, size);
}

void ojr_call_reseed(ojr_generator *g, uint32_t *seed, int size) {
    int id = g->algorithm;
    void (*f)(ojr_generator *, uint32_t *, int);

    if (0 == id) id = 1;
    f = ojr_algorithms[id - 1]->reseed;
    if (f) { (*f)(g, seed, size); }
    else ojr_default_reseed(g, seed, size);
}

void ojr_call_refill(ojr_generator *g) {
    int id = g->algorithm;
    void (*f)(ojr_generator *);

    if (0 == id) id = 1;
    f = ojr_algorithms[id - 1]->refill;
    if (f) { (*f)(g); }
}


// If the algorithm has no reseed function, this will be called.
void ojr_default_reseed(ojr_generator *g, uint32_t *seed, int size) {
    int i, j = 0;

    for (i = 0; i < g->statesize; ++i) {
        g->state[i] ^= seed[j];
        if (++j >= size) j = 0;
    }
}

void ojr_default_seed(ojr_generator *g, uint32_t *seed, int size) {
    int i, x, j = 0;

    x = 232497429;
    for (i = 0; i < g->statesize; ++i) {
        x = (69069 * x) + 764385 + seed[j];
        g->state[i] = x;
        if (++j >= size) j = 0;
    }
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
