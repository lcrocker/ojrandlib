/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Basic Java example.
 */

import com.onejoker.randlib.*;

public class Hello {
    static { System.loadLibrary("ojrand"); }

    private static final int[] gSeed = { 0x123, 0x234, 0x345, 0x456 };

    public static void main(String[] args) {
        Generator g = new Generator("mt19937");
        g.seed(gSeed);

        for (int i = 0; i < 20; ++i) {
            System.out.printf("%12d ", g.next32());
            if (4 == (i % 5)) System.out.printf("\n");
        }
    }
}
