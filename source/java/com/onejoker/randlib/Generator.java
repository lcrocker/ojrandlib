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

public class Generator {
    private ByteBuffer mS, mState, mBuf, mSeed = null;
    private static int mStructSize;
    static { mStructSize = nStructSize(); }

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
        nInit(mS);
        nSetAlgorithm(mS, id);

        int s = nAlgorithmStateSize(id);
        this.mState = ByteBuffer.allocateDirect(4 * s);
        nSetState(mS, mState, s);

        s = nAlgorithmBufSize(id);
        this.mBuf = ByteBuffer.allocateDirect(4 * s);
        nSetBuf(mS, mBuf, s);
        nCallOpen(mS);
    }
    public Generator(String name) { this(Generator.algorithmID(name)); }
    public Generator() { this(1); }

    public void seed(int[] s) {
        ByteBuffer b = ByteBuffer.allocateDirect(4 * s.length);
        b.order(ByteOrder.LITTLE_ENDIAN);
        for (int i = 0; i < s.length; ++i) { b.putInt(4 * i, s[i]); }
        nCallSeed(mS, b, s.length);
        nSetSeeded(mS, 1);
    }

    public void seed(int s) {
        ByteBuffer b = ByteBuffer.allocateDirect(4);
        b.order(ByteOrder.LITTLE_ENDIAN);
        b.putInt(0, s);
        nCallSeed(mS, b, 1);
        nSetSeeded(mS, 1);
    }

    public void seed() {
        int s = nAlgorithmSeedSize(nGetAlgorithm(mS));
        ByteBuffer b = ByteBuffer.allocateDirect(4 * s);
        nGetSystemEntropy(b, s);
        nCallSeed(mS, b, s);
        nSetSeeded(mS, 1);
    }

    public void reseed() {
        int s = nAlgorithmSeedSize(nGetAlgorithm(mS));
        ByteBuffer b = ByteBuffer.allocateDirect(4 * s);
        nGetSystemEntropy(b, s);
        nCallReseed(mS, b, s);
    }
    public int next16() { return nNext16(mS); }
    public int next32() { return nNext32(mS); }
    public long next64() { return nNext64(mS); }

    public double nextDouble() { return nNextDouble(mS); }
    public double nextSignedDouble() { return nNextSignedDouble(mS); }
    public double nextGaussian() { return nNextGaussian(mS); }

    public int rand(int limit) { return nRand(mS, limit); }
    public void discard(int count) { nDiscard(mS, count); }

    private static native int nStructSize();
    private static native void nInit(ByteBuffer b);
    private static native int nAlgorithmCount();
    private static native int nAlgorithmID(String name);
    private static native String nAlgorithmName(int id);
    private static native int nAlgorithmSeedSize(int id);
    private static native int nAlgorithmStateSize(int id);
    private static native int nAlgorithmBufSize(int id);
    private static native void nGetSystemEntropy(ByteBuffer b, int count);

    private static native int nSetSeeded(ByteBuffer b, int s);
    private static native int nGetAlgorithm(ByteBuffer b);
    private static native void nSetAlgorithm(ByteBuffer b, int id);
    private static native void nSetState(ByteBuffer b, ByteBuffer s, int size);
    private static native void nSetBuf(ByteBuffer b, ByteBuffer buf, int size);

    private static native void nCallOpen(ByteBuffer b);
    private static native void nCallClose(ByteBuffer b);
    private static native void nCallSeed(ByteBuffer b, ByteBuffer s, int c);
    private static native void nCallReseed(ByteBuffer b, ByteBuffer s, int c);
    private static native void nCallRefill(ByteBuffer b);

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
