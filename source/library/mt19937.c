/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
 *
 * This is the Mersenne Twister algorithm by Takuji Nishimura and Makoto
 * Matsumoto. I believe the implementation is sufficiently different to avoid
 * copyright issues, so the CC0 dedication applies.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ojrandlib.h"

static void _ojr_mt19937_seed(ojr_generator *g, uint32_t *seed, int size) {
    int i, j, k, n = g->statesize;
    uint32_t *s = g->state;

    if (1 == size) s[0]= seed[0];
    else s[0] = 19650218U;

    for (i = 1; i < n; ++i) {
        s[i] = (1812433253U * (s[i-1] ^ (s[i-1] >> 30)) + i);
    }
    g->bptr = g->buf;
    if (1 == size) return;

    i = 1;
    j = 0;
    for (k = (n > size) ? n : size; k > 0; --k) {
        s[i] = (s[i] ^ ((s[i-1] ^ (s[i - 1] >> 30)) * 1664525U)) + seed[j] + j;
        ++i;    if (i >= n) { s[0] = s[n-1]; i = 1; }
        ++j;    if (j >= size) j = 0;
    }
    for (k = n - 1; k > 0; --k) {
        s[i] = (s[i] ^ ((s[i - 1] ^ (s[i-1] >> 30)) * 1566083941U)) - i;
        ++i;    if (i >= n) { s[0] = s[n-1]; i = 1; }
    }
    s[0] = 0x80000000;
}

static void _ojr_mt19937_reseed(ojr_generator *g, uint32_t *seed, int size) {
    ojr_default_reseed(g, seed, size);
    g->state[0] = 0x80000000;
}

static void _ojr_mt19937_refill(struct _ojr_generator *g) {
    int i, j, k, n = g->statesize;
    uint32_t y, m, *s = g->state, *bp = g->buf + g->bufsize;

    for (i = 0; i < n; ++i) {
        j = i + 1;      if (j >= n) j -= n;
        k = i + 397;    if (k >= n) k -= n;

        m = (s[j] & 1) ? 0x9908b0df : 0;
        s[i] = m ^ s[k] ^ (((s[i] & 0x80000000) | (s[j] & 0x7FFFFFFF)) >> 1);
    }
    for (i = 0; i < n; ++i) {
        y = s[i] ^ (s[i] >> 11);
        y ^= (y << 7) & 0x9d2c5680U;
        y ^= (y << 15) & 0xefc60000U;
        *--bp = y ^ (y >> 18);
    }
}

ojr_algorithm ojr_algorithm_mt19937 = {
    "mt19937",
    16, 624, 624,
    NULL, NULL,
    _ojr_mt19937_seed,
    _ojr_mt19937_reseed,
    _ojr_mt19937_refill,
};
