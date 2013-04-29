/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Basic test for pseudo-random number generator.
 */

#include <cstdlib>
#include <cstdio>
using namespace std;

#include "ojrandlib.h"
using namespace oj;

static const uint32_t mtseed[] = { 0x123, 0x234, 0x345, 0x456 };

int main(int argc, char *argv[]) {
    Seed s(mtseed, mtseed + 4);
    Generator g("mt19937");
    g.seed(s);

    for (int i = 0; i < 100; ++i) {
        printf("%10u ", g.next32());
        if (4 == (i % 5)) printf("\n");     
    }
    return EXIT_SUCCESS;
}
