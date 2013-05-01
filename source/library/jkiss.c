/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Pseudo-random number generator based on the public domain JKISS by
 * David Jones of the UCL Bioinformatics Group.
 * <http://www.cs.ucl.ac.uk/staff/d.jones/GoodPracticeRNG.pdf>.
 *
 * This is also a canonical example of how to implement a new algorithm
 * for the library: first, implement the five functions open, close,
 * seed, reseed, and refill. The first two can do nothing if you don't
 * need them, and the third and fourth have handy defaults. The real
 * work is in refill(), which needs to simply fill a buffer full of
 * random bits. Then you have to define the algorithm info structure and
 * fill it, then add it to the list at libmain.c. That's it--all the rest
 * you get for free.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ojrandlib.h"

static void _ojr_jkiss_seed(ojr_generator *g) {
    if (g->seedsize < 4) {
        g->state[0] = 123456789;
        g->state[1] = 987654321;
        g->state[2] = 43219876;
        g->state[3] = 6543217;
    }
    ojr_default_seed(g);
    if (0 == g->state[1]) g->state[1] = 1;
    g->state[3] = g->state[3] % 698769068 + 1;
}

static void _ojr_jkiss_reseed(ojr_generator *g, int value) {
    ojr_default_reseed(g, value);
    if (0 == g->state[1]) g->state[1] = 1;
    g->state[3] = g->state[3] % 698769068 + 1;
}

static void _ojr_jkiss_refill(ojr_generator *g) {
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

ojr_algorithm ojr_algorithm_jkiss = {
    "jkiss",
    4, 4,
    256,                /* Any reasonable value is OK here */
    NULL, NULL,         /* No need for open() or close() */
    _ojr_jkiss_seed,    /* Apply seed to empty state vector */
    _ojr_jkiss_reseed,  /* Add new seed to existing state */
    _ojr_jkiss_refill,  /* Produce a bufferfull of randomness */
};
