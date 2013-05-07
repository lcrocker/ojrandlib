/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
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

static void _ojr_mwc256_seed(ojr_generator *g, uint32_t *seed, int size) {
    ojr_default_seed(g, seed, size);
    g->state[256] = 362436;
}

static void _ojr_mwc256_refill(ojr_generator *g) {
    int i;
    uint64_t t;
    uint32_t *s = g->state, c = g->state[256], *bp = g->buf + g->bufsize;
    assert(257 == g->statesize);

    for (i = 0; i < 256; ++i) {
        t = 809430660ULL * s[i] + c;
        c = t >> 32;
        *--bp = (uint32_t)t;
    }
    g->state[256] = c;
}

ojr_algorithm ojr_algorithm_mwc256 = {
    "mwc256",
    16, 257, 256,
    NULL, NULL,
    _ojr_mwc256_seed,
    NULL,
    _ojr_mwc256_refill,
};
