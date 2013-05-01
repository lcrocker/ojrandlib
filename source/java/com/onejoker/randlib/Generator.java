/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 *
 * Java language binding.
 */

package com.onejoker.randlib;
import java.nio.*;
import java.util.ArrayList;
import java.util.Iterator;

public class Generator {
    private ByteBuffer mBuf;
    private static int mStructSize;

    private static native int nStructSize();
    static { mStructSize = nStructSize(); }

    public Generator(int id) {
        mBuf = ByteBuffer.allocateDirect(mStructSize);
        nNew(mBuf, size);
    }
}
