/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Basic test for pseudo-random number generator.
 */

#include <stdlib.h>
#include <stdio.h>

#include "ojrandlib.h"

int main(int argc, char *argv[]) {
    int i;
    ojr_generator *g = ojr_new(0);
    ojr_seed(g, NULL, 0);

    for (i = 0; i < 100; ++i) {
        printf("%10u ", ojr_next32(g));
        if (4 == (i % 5)) printf("\n");
    }

    ojr_close(g);
    return EXIT_SUCCESS;
}
