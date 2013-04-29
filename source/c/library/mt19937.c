/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * This code for the Mersenne Twister algorithm is based on the original
 * reference code "mt19937ar.c" by Takuji Nishimura and Makoto Matsumoto.
 *
 * Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
 * All rights reserved.                          
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. The names of its contributors may not be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Any feedback is very welcome.
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
 * email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "ojrandlib.h"

extern void _ojr_default_reseed(ojr_generator *g);
extern void _ojr_default_seed(ojr_generator *g);

static void _ojr_mt19937_open(ojr_generator *g) { return; }
static void _ojr_mt19937_close(ojr_generator *g) { return; }

/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static void _ojr_mt19937_seed(ojr_generator *g) {
    int i, j, k;
    uint32_t *mt = g->state;
    assert(624 == g->statesize);

    mt[0] = 19650218U;
    for (i = 1; i < N; ++i) {
        mt[i] = (1812433253U * (mt[i - 1] ^ (mt[i - 1] >> 30)) + i); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
    }
    i = 1; j = 0;
    k = (N > g->seedsize) ? N : g->seedsize;
    while (k) {
        mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1664525U))
          + g->seed[j] + j; /* non linear */

        ++i; ++j;
        if (i >= N) { mt[0] = mt[N - 1]; i = 1; }
        if (j >= g->seedsize) j = 0;
        --k;
    }
    for (k = N - 1; k > 0; --k) {
        mt[i] = (mt[i] ^ ((mt[i - 1] ^ (mt[i - 1] >> 30)) * 1566083941U))
          - i; /* non linear */
        ++i;
        if (i >= N) { mt[0] = mt[N - 1]; i = 1; }
    }
    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */
}

static void _ojr_mt19937_reseed(ojr_generator *g) {
    assert(624 == g->statesize);
    _ojr_default_reseed(g);
    g->state[0] = 0x80000000UL;
}

static void _ojr_mt19937_refill(struct _ojr_generator *g) {
    int i, kk;
    uint32_t y, *mt = g->state, *bp = g->buf + g->bufsize;
    static uint32_t mag01[2] = {0x0U, MATRIX_A};
    assert(624 == g->statesize && 624 == g->bufsize);

    for (kk = 0; kk < N-M; ++kk) {
        y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
        mt[kk] = mt[kk + M] ^ (y >> 1) ^ mag01[y & 0x1U];
    }
    for (; kk < N - 1; ++kk) {
        y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
        mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ mag01[y & 0x1U];
    }
    y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
    mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ mag01[y & 0x1U];

    for (i = 0; i < N; ++i) {  
        y = mt[i];
        /* Tempering */
        y ^= (y >> 11);
        y ^= (y << 7) & 0x9d2c5680U;
        y ^= (y << 15) & 0xefc60000U;
        y ^= (y >> 18);
        *--bp = y;
    }
}

ojr_algorithm ojr_algorithm_mt19937 = {
    "mt19937",
    16, 624, 624,
    _ojr_mt19937_open,
    _ojr_mt19937_close,
    _ojr_mt19937_seed,
    _ojr_mt19937_reseed,
    _ojr_mt19937_refill,
};
