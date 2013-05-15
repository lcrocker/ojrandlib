/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Basic C usage example for pseudo-random number generator.
 */

#include <stdlib.h>
#include <stdio.h>

#include "ojrandlib.h"

int main(int argc, char *argv[]) {
    printf("  %d  %d  %d  %f\n",
        ojr_rand(NULL, 100), ojr_next16(NULL), ojr_next32(NULL),
        ojr_next_double(NULL));
    return 0;
}
