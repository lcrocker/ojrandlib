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
        ojr_rand(DEFGEN, 100), ojr_next16(DEFGEN), ojr_next32(DEFGEN),
        ojr_next_double(DEFGEN));
    return 0;
}
