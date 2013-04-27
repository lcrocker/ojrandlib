/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * Test for pseudo-random number generator.
 */

#include <stdlib.h>
#include <stdio.h>

#include "ojrandlib.h"

int main(int argc, char *argv[]) {
    ojr_generator *g = ojr_new(NULL);
    ojr_seed(g, NULL, 0);
    printf("%d %d %d\n", ojr_next16(g), ojr_next32(g), ojr_rand(g, 52));
    ojr_close(g);
    return EXIT_SUCCESS;
}
