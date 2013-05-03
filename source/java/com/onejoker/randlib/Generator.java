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
    private ByteBuffer mS, mState, mBuf, mSeed = null;
    private static int mStructSize;
    static { mStructSize = nStructSize(); }
    private int mStateSize, mBufSize, mSeedSize, mAlgorithm;

    public static int algorithmCount() { return nAlgorithmCount(); }
    public static int algorithmID(String name) { return nAlgorithmID(name); }
    public static String algorithmName(int id) { return nAlgorithmName(id); }
    public static int algorithmSeedSize(int id) { return nAlgorithmSeedSize(id); }
    public static int algorithmStateSize(int id) { return nAlgorithmStateSize(id); }
    public static int algorithmBufSize(int id) { return nAlgorithmBufSize(id); }

    public static ByteBuffer getSystemEntropy(int count) {
        ByteBuffer b = ByteBuffer.allocateDirect(4 * count);
        nGetSystemEntropy(b, count);
        return b;
    }

    public Generator(int id) {
        mS = ByteBuffer.allocateDirect(mStructSize);
        mAlgorithm = id;
        nInit(mS);
        nSetAlgorithm(mS, mAlgorithm);

        this.mStateSize = nAlgorithmStateSize(id);
        this.mState = ByteBuffer.allocateDirect(4 * mStateSize);
        nSetState(mS, mState, mStateSize);

        this.mBufSize = nAlgorithmBufSize(id);
        this.mBuf = ByteBuffer.allocateDirect(4 * mBufSize);
        nSetBuf(mS, mBuf, mBufSize);
        nCallOpen(mS);
    }
    public Generator(String name) { this(Generator.algorithmID(name)); }
    public Generator() { this(1); }

    public static void _seed(ByteBuffer b, ByteBuffer s) {
        boolean first = (0xb1e55ed0 == nGetStatus(b));
        nSetSeed(b, s, (b.capacity() / 4));
        nCallSeed(b);

        if (first) nSetStatus(b, 0xb1e55ed2);
        else nSetStatus(b, 0xb1e55ed2);
    }

    public void seed(ArrayList<Integer> s) {
        ByteBuffer b = ByteBuffer.allocateDirect(4 * s.size());
        b.order(ByteOrder.LITTLE_ENDIAN);
        for (int i = 0; i < s.size(); ++i) { b.putInt(4 * i, s.get(i)); }
        Generator._seed(mS, b);
    }

    public void seed(int s) {
        ByteBuffer b = ByteBuffer.allocateDirect(4);
        b.order(ByteOrder.LITTLE_ENDIAN);
        b.putInt(0, s);
        Generator._seed(mS, b);
    }

    public void seed() {
        int s = nAlgorithmSeedSize(mAlgorithm);
        ByteBuffer b = ByteBuffer.allocateDirect(4 * s);
        nGetSystemEntropy(b, s);
        Generator._seed(mS, b);
    }

    private static native int nStructSize();
    private static native void nInit(ByteBuffer b);
    private static native int nAlgorithmCount();
    private static native int nAlgorithmID(String name);
    private static native String nAlgorithmName(int id);
    private static native int nAlgorithmSeedSize(int id);
    private static native int nAlgorithmStateSize(int id);
    private static native int nAlgorithmBufSize(int id);
    private static native void nGetSystemEntropy(ByteBuffer b, int count);

    private static native int nGetStatus(ByteBuffer b);
    private static native void nSetStatus(ByteBuffer b, int s);
    private static native void nSetSeed(ByteBuffer b, ByteBuffer s, int size);
    private static native void nSetState(ByteBuffer b, ByteBuffer s, int size);
    private static native void nSetBuf(ByteBuffer b, ByteBuffer buf, int size);
    private static native void nSetAlgorithm(ByteBuffer b, int id);

    private static native void nCallOpen(ByteBuffer b);
    private static native void nCallClose(ByteBuffer b);
    private static native void nCallSeed(ByteBuffer b);
    private static native void nCallReseed(ByteBuffer b, int v);
    private static native void nCallRefill(ByteBuffer b);
    private static native void nDefaultReseed(ByteBuffer b, int s);

    private static native int nNext16(ByteBuffer b);
    private static native int nNext32(ByteBuffer b);
    private static native long nNext64(ByteBuffer b);
    private static native double nNextDouble(ByteBuffer b);
    private static native double nNextSignedDouble(ByteBuffer b);
    private static native double nNextGaussian(ByteBuffer b);

    private static native int nRand(ByteBuffer b, int limit);
    private static native void nDiscard(ByteBuffer b, int count);
    /* Shuffles? */
}
