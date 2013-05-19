/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * This code is based on George Marsaglia's Ziggurat algorithm, with
 * various improvements from Jurgen A. Doornik and me. I believe it is
 * sufficiently dissimilar to any pre-existing implementation that there
 * should be no copyright problems, so my CC0 dedication applies.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "ojrandlib.h"
#include "zigtables.h"

// The sizes of 128 and 256 from GM's paper are good, but I recalculated
// the constants here and built fixed tables based on them.

#define ZNR128 3.4426198558966521214
#define ZNV128 0.0099125630353364610791
#define ZER256 7.6971174701310497140
#define ZEV256 0.0039496598225815572200

double ojr_next_exponential(ojr_generator *g) {
    uint64_t r;
    int i;
    double x, u0, f0, f1;

    while (1) {
        r = ojr_next64(g);
        i = r & 0xFF;
        r = (r >> 8) & 0xFFFFFFFFFFFFFULL;
        r |= 0x3FF0000000000000ULL;
        u0 = *(double *)(&r) - 1.0;

        if (u0 < zer[i]) return u0 * zex[i];
        if (0 == i) return ZER256 - log(ojr_next_double(g));

        x = u0 * zex[i];
        f0 = exp(x - zex[i]);
        f1 = exp(x - zex[i+1]);
        if (f1 + ojr_next_double(g) * (f0 - f1) < 1.0) return x;
    }
}

double ojr_next_normal(ojr_generator *g) {
    uint64_t r;
    int i, sign;
    double x, y, a, u0, f0, f1;

    while (1) {
        do {
            r = ojr_next64(g);
            sign = (int)r & 1;
            i = (r >> 1) & 0x7F;
            r >>= 12;
        } while (sign && 0LL == r);

        r |= 0x3FF0000000000000ULL;
        a = *(double *)(&r) - 1.0;
        u0 = sign ? -a : a;

        if (a < znr[i]) return u0 * znx[i];
        if (0 == i) {
            do {
                x = log(ojr_next_double(g)) / ZNR128;
                y = log(ojr_next_double(g));
            } while (-2.0 * y < x * x);
            return sign ? x - ZNR128 : ZNR128 - x;
        }
        x = u0 * znx[i];
        f0 = exp(-0.5 * (znx[i] * znx[i] - x * x));
        f1 = exp(-0.5 * (znx[i+1] * znx[i+1] - x * x));
        if (f1 + ojr_next_double(g) * (f0 - f1) < 1.0) return x;
    }
}
