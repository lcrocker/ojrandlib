/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Pseudo-random number generator based on the public domain JKISS by
 * David Jones of the UCL Bioinformatics Group.
 * <http://www.cs.ucl.ac.uk/staff/d.jones/GoodPracticeRNG.pdf>.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ojrandlib.h"

static void _ojr_jkiss127_seed(ojr_generator *g, uint32_t *seed, int size) {
    ojr_default_seed(g, seed, size);
    if (0 == g->state[1]) g->state[1] = 1;
    g->state[3] = g->state[3] % 698769068 + 1;
}

static void _ojr_jkiss127_reseed(ojr_generator *g, uint32_t *seed, int size) {
    ojr_default_reseed(g, seed, size);
    if (0 == g->state[1]) g->state[1] = 1;
    g->state[3] = g->state[3] % 698769068 + 1;
}

static void _ojr_jkiss127_refill(ojr_generator *g) {
    int i;
    uint32_t *s = g->state, *bp = g->buf + g->bufsize;
    uint64_t t;
    assert(4 == g->statesize);

    for (i = 0; i < g->bufsize; ++i) {
        s[0] = 314527869 * s[0] + 1234567;
        s[1] ^= s[1] << 5;
        s[1] ^= s[1] >> 7;
        s[1] ^= s[1] << 22;
        t = 4294584393ULL * s[2] + s[3];
        s[3] = t >> 32;
        s[2] = t;
        *--bp = s[0] + s[1] + s[2];
    }
}

/* Every algorithm must define this structure publically, and add its address
 * to the list in libmain.c.
 */

ojr_algorithm ojr_algorithm_jkiss127 = {
    "jkiss127",
    4, 4,
    256,                /* Any reasonable value is OK here */
    NULL, NULL,         /* No need for open() or close() */
    _ojr_jkiss127_seed,    /* Apply seed to empty state vector */
    _ojr_jkiss127_reseed,  /* Add new seed to existing state */
    _ojr_jkiss127_refill,  /* Produce a bufferfull of randomness */
};
