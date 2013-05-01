/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * The simple C API uses open / close to create generator objects. These
 * allocate memory with malloc(), so they are generally not used by the
 * higher level language bindings.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "ojrandlib.h"

ojr_generator *ojr_open(int id) {
    ojr_algorithm *ap;
    ojr_generator *gp;
    uint32_t *sp, *bp;

    if (0 == id) id = 1;
    if (id > ojr_algorithm_count()) return NULL;
    ap = _ojr_algorithms[id - 1];

    gp = malloc(sizeof(ojr_generator));
    if (gp) {
        sp = calloc(ap->statesize, 4);
        if (sp) {
            bp = calloc(ap->bufsize, 4);
            if (bp) {
                _ojr_init(gp);
                _ojr_set_algorithm(gp, id);
                _ojr_set_state(gp, sp, ap->statesize);
                _ojr_set_buffer(gp, bp, ap->bufsize);
                _ojr_call_open(gp);
                return gp;
            }
            free(sp);
        }
        free(gp);
    }
    return NULL;
}

/* We're done with this generator. Free up allocated memory.
 */

void ojr_close(ojr_generator *g) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    assert(g->state && g->buf);

    _ojr_call_close(g);
    if (g->seed) free(g->seed);
    free(g->buf);
    free(g->state);
    free(g);
}

/* Common code for seed & reseed functions. Create the seed buffer in the
 * generator object, fill it from the passed-in values. The API is a little
 * weird: if <byval> is true, then <size> is an integer seed. If <seed> is
 * NULL, then we want to use system entropy: <size> words, or if <size> is 0,
 * use the algorithm's recommended seed size. Finally, only if <seed> and
 * <size> are both set do we use them as is.
 */

static int _ojr_new_seed(ojr_generator *g, uint32_t *seed, int size, int byval) {
    uint32_t *sp;
    int first = (NULL == g->seed);

    if (byval) {
        if (! (seed = malloc(4))) return 0;
        *seed = size;
        size = 1;
    } else {
        if (NULL == seed && 0 == size) { size = g->ap->seedsize; }
    }
    if (! (sp = malloc(4 * size))) return 0;
    if (! first) { free(g->seed); }

    if (NULL == seed) ojr_get_system_entropy(sp, size);
    else memmove(sp, seed, 4 * size);
    _ojr_set_seed(g, sp, size);

    if (first) g->_status = 0xb1e55ed2;
    else g->_status = 0xb1e55ed3;
    return 1;
}

int ojr_int_seed(ojr_generator *g, int value) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    if (! _ojr_new_seed(g, NULL, value, 1)) return 0;
    _ojr_call_seed(g);
    return g->seedsize;
}

int ojr_good_seed(ojr_generator *g) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    if (! _ojr_new_seed(g, NULL, 0, 0)) return 0;
    _ojr_call_seed(g);
    return g->seedsize;
}

int ojr_array_seed(ojr_generator *g, uint32_t *seed, int size) {
    assert(0xb1e55ed0 == (g->_status & 0xfffffff0));
    assert(0 != size && 0 != seed);

    if (! _ojr_new_seed(g, seed, size, 0)) return 0;
    _ojr_call_seed(g);
    return g->seedsize;
}

void ojr_reseed(ojr_generator *g) {
    uint32_t s;
    assert(0xb1e55ed2 == (g->_status & 0xfffffff2));

    ojr_get_system_entropy(&s, 1);
    if (NULL == g->ap->reseed) ojr_default_reseed(g, s);
    else _ojr_call_reseed(g, s);

    _ojr_call_refill(g);
    g->bptr = g->buf + g->bufsize;
    g->_status = 0xb1e55ed3;
}
