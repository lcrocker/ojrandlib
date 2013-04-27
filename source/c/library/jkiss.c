/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * Pseudo-random number generator based on the public domain JKISS by
 * David Jones of the UCL Bioinformatics Group.
 * <http://www.cs.ucl.ac.uk/staff/d.jones/GoodPracticeRNG.pdf>.
 *
 * To add an algorithm, you have to implement three functions like those
 * below and add them to the front of ojrmain.c.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ojrandlib.h"

void _ojr_default_reseed(ojr_generator *g, uint32_t *seed, int size);
void _ojr_default_seed(ojr_generator *g);

extern void _ojr_jkiss_seed(ojr_generator *g) {
    assert(0xca11ab1e == g->_status || 0x5eed1e55 == g->_status);
    assert(4 == g->statesize && 4 == g->seedsize);

    _ojr_default_seed(g);
    if (0 == g->state[1]) g->state[1] = 1;
    g->state[3] = g->state[3] % 698769068 + 1;
}

extern void _ojr_jkiss_reseed(ojr_generator *g, uint32_t *seed, int size) {
    assert(0xca11ab1e == g->_status || 0x5eed1e55 == g->_status);
    assert(4 == g->statesize && 4 == g->seedsize);

    _ojr_default_reseed(g, seed, size);
    if (0 == g->state[1]) g->state[1] = 1;
    g->state[3] = g->state[3] % 698769068 + 1;    
}

extern void _ojr_jkiss_refill(struct _ojr_generator *g) {
    int i;
    uint64_t t;
    assert(0xca11ab1e == g->_status || 0x5eed1e55 == g->_status);
    assert(4 == g->statesize);

    for (i = 0; i < g->bufsize; ++i) {
        g->state[0] = 314527869 * g->state[0] + 1234567;
        g->state[1] ^= g->state[1] << 5;
        g->state[1] ^= g->state[1] >> 7;
        g->state[1] ^= g->state[1] << 22;
        t = 4294584393ULL * g->state[2] + g->state[3];
        g->state[3] = t >> 32;
        g->state[2] = t;
        ((uint32_t *)(g->buf))[i] = g->state[0] + g->state[1] + g->state[2];
    }
}
