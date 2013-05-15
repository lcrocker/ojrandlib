/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * This code is based on ZIGNOR by Jurgen A. Doornik: "An Improved Ziggurat
 * Method to Generate Normal Random Samples", mimeo, Nuffield College,
 * University of Oxford <www.doornik.com/research>. This notice should be
 * maintained in modified versions of the code. No warranty is given regarding
 * the correctness of this code.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "ojrandlib.h"

#define ZIGNOR_C 128
#define ZIGNOR_R 3.442619855899
#define ZIGNOR_V 9.91256303526217e-3

static double s_adZigX[ZIGNOR_C + 1], s_adZigR[ZIGNOR_C];
static int _zignor_table_initialized = 0;

static void _zignor_init(int iC, double dR, double dV) {
    int i;
    double f;

    f = exp(-0.5 * dR * dR);
    s_adZigX[0] = dV / f; /* [0] is bottom block: V / f(R) */
    s_adZigX[1] = dR;
    s_adZigX[iC] = 0;

    for (i = 2; i < iC; ++i) {
        s_adZigX[i] = sqrt(-2 * log(dV / s_adZigX[i - 1] + f));
        f = exp(-0.5 * s_adZigX[i] * s_adZigX[i]);
    }
    for (i = 0; i < iC; ++i) {
        s_adZigR[i] = s_adZigX[i + 1] / s_adZigX[i];
    }
}

static double _normal_tail(ojr_generator *g, double dmin, int neg) {
    double x, y;

    do {
        x = log(ojr_next_double(g)) / dmin;
        y = log(ojr_next_double(g));
    } while (-2 * y < x * x);
    return neg ? x - dmin : dmin - x;
}

double ojr_next_gaussian(ojr_generator *g) {
    int i, sign;
    double x, a, u, f0, f1;
    uint64_t r64;

    if (! _zignor_table_initialized) {
        _zignor_init(ZIGNOR_C, ZIGNOR_R, ZIGNOR_V);
        _zignor_table_initialized = 1;
    }
    while (1) {
        do {
            r64 = ojr_next64(g);
            sign = (int)r64 & 1;
            i = (r64 >> 1) & 0x7F;
            r64 >>= 12;
        } while (sign && 0LL == r64);

        r64 |= 0x3FF0000000000000;
        a = *(double *)(&r64) - 1.0;
        u = sign ? -a : a;

        if (a < s_adZigR[i]) return u * s_adZigX[i];
        if (0 == i) return _normal_tail(g, ZIGNOR_R, sign);

        x = u * s_adZigX[i];
        f0 = exp( -0.5 * (s_adZigX[i] * s_adZigX[i] - x * x) );
        f1 = exp( -0.5 * (s_adZigX[i + 1] * s_adZigX[i + 1] - x * x) );

        if (f1 + ojr_next_double(g) * (f0 - f1) < 1.0) return x;
    }
}
