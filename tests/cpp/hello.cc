/* OneJoker RNG library <http://lcrocker.github.io/onejoker/randlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Basic C++ usage example for pseudo-random number generator.
 */

#include <iostream>
using namespace std;

#include "ojrandlib.h"
using namespace oj;

static const uint32_t mtseed[] = { 0x123, 0x234, 0x345, 0x456 };

int main(int argc, char *argv[]) {
    Seed s(mtseed, mtseed + 4);
    Generator g("mt19937");
    g.seed(s);

    cout << "  " << g.rand(100) << "  " << g.next16()     <<
            "  " << g.next32()  << "  " << g.nextDouble() << "\n";

    return 0;
}
