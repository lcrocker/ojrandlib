/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Generator based on George Marsaglia's Multiply-with-carry.
 * <http://en.wikipedia.org/wiki/Multiply-with-carry>
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ojrandlib.h"

static void _ojr_mwc8222_reseed(ojr_generator *g, uint32_t *seed, int size) {
    int j = 0;

    for (int i = 0; i < g->bufsize; ++i) {
        g->buf[i] ^= seed[j];
        if (++j >= size) j = 0;
    }
}

static void _ojr_mwc8222_seed(ojr_generator *g, uint32_t *seed, int size) {
    int x = 232497429, j = 0;

    for (int i = 0; i < g->bufsize; ++i) {
        x = (69069 * x) + 764385 + seed[j];
        g->buf[i] = x;
        if (++j >= size) j = 0;
    }
    g->state[0] = 362436;
}

static void _ojr_mwc8222_refill(ojr_generator *g) {
    uint64_t t;
    assert(1 == g->statesize && 256 == g->bufsize);
    uint32_t *s = g->buf, c = g->state[0];

    for (int i = 255; i >= 0; --i) {
        t = 809430660ULL * s[i] + c;
        c = t >> 32;
        s[i] = (uint32_t)t;
    }
    g->state[0] = c;
}

ojr_algorithm ojr_algorithm_mwc8222 = {
    "mwc8222",
    16, 1, 256,         // Output buffer is state vector
    NULL, NULL,
    _ojr_mwc8222_seed,
    _ojr_mwc8222_reseed,
    _ojr_mwc8222_refill,
};
