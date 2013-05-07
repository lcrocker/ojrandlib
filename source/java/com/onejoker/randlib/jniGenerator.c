/* OneJoker RNG library <http://lcrocker.github.io/onejoker/ojrandlib>
 *
 * To the extent possibile under law, Lee Daniel Crocker has waived all
 * copyright and related or neighboring rights to this work.
 * <http://creativecommons.org/publicdomain/zero/1.0/>
 */

#include <stdlib.h>
#include <stdint.h>
#include <jni.h>

#include "ojrandlib.h"

JNIEXPORT jint JNICALL Java_com_onejoker_randlib_Generator_nStructSize
(JNIEnv *env, jclass cls) {
	return sizeof(ojr_generator);
}

JNIEXPORT void JNICALL Java_com_onejoker_randlib_Generator_nInit
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    ojr_init((ojr_generator *)ptr);
}

JNIEXPORT jint JNICALL Java_com_onejoker_randlib_Generator_nAlgorithmCount
(JNIEnv *env, jclass cls) {
	return ojr_algorithm_count();
}

JNIEXPORT jint JNICALL Java_com_onejoker_randlib_Generator_nAlgorithmID
(JNIEnv *env, jclass cls, jstring name) {
	const char *np = (*env)->GetStringUTFChars(env, name, 0);
	int id = ojr_algorithm_id(np);
	(*env)->ReleaseStringUTFChars(env, name, np);
	return id;
}

JNIEXPORT jstring JNICALL Java_com_onejoker_randlib_Generator_nAlgorithmName
(JNIEnv *env, jclass cls, jint id) {
	const char *np = ojr_algorithm_name(id);
	return (*env)->NewStringUTF(env, np);
}

JNIEXPORT jint JNICALL Java_com_onejoker_randlib_Generator_nAlgorithmSeedSize
(JNIEnv *env, jclass cls, jint id) {
	return ojr_algorithm_seedsize(id);
}

JNIEXPORT jint JNICALL Java_com_onejoker_randlib_Generator_nAlgorithmStateSize
(JNIEnv *env, jclass cls, jint id) {
	return ojr_algorithm_statesize(id);
}

JNIEXPORT jint JNICALL Java_com_onejoker_randlib_Generator_nAlgorithmBufSize
(JNIEnv *env, jclass cls, jint id) {
	return ojr_algorithm_bufsize(id);
}

JNIEXPORT void JNICALL Java_com_onejoker_randlib_Generator_nGetSystemEntropy
(JNIEnv *env, jclass cls, jobject b, jint c) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
	ojr_get_system_entropy((uint32_t *)ptr, c);
}

JNIEXPORT void JNICALL Java_com_onejoker_randlib_Generator_nSetSeeded
(JNIEnv *env, jclass cls, jobject b, jint s) {  
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    ojr_set_seeded((ojr_generator *)ptr, s);
}

JNIEXPORT jint JNICALL Java_com_onejoker_randlib_Generator_nGetAlgorithm
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    return ojr_get_algorithm((ojr_generator *)ptr);
}

JNIEXPORT void JNICALL Java_com_onejoker_randlib_Generator_nSetAlgorithm
(JNIEnv *env, jclass cls, jobject b, jint id) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    ojr_set_algorithm((ojr_generator *)ptr, id);
}

JNIEXPORT void JNICALL Java_com_onejoker_randlib_Generator_nSetState
(JNIEnv *env, jclass cls, jobject b, jobject state, jint size) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    jbyte *sp = (*env)->GetDirectBufferAddress(env, state);
    ojr_set_state((ojr_generator *)ptr, (uint32_t *)sp, size);
}

JNIEXPORT void JNICALL Java_com_onejoker_randlib_Generator_nSetBuf
(JNIEnv *env, jclass cls, jobject b, jobject buf, jint size) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    jbyte *bp = (*env)->GetDirectBufferAddress(env, buf);
    ojr_set_buffer((ojr_generator *)ptr, (uint32_t *)bp, size);
}

JNIEXPORT void JNICALL Java_com_onejoker_randlib_Generator_nCallOpen
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    ojr_call_open((ojr_generator *)ptr);
}

JNIEXPORT void JNICALL Java_com_onejoker_randlib_Generator_nCallClose
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    ojr_call_close((ojr_generator *)ptr);
}

JNIEXPORT void JNICALL Java_com_onejoker_randlib_Generator_nCallSeed
(JNIEnv *env, jclass cls, jobject b, jobject s, jint c) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    jbyte *sp = (*env)->GetDirectBufferAddress(env, s);
    ojr_call_seed((ojr_generator *)ptr, (uint32_t *)sp, c);
}

JNIEXPORT void JNICALL Java_com_onejoker_randlib_Generator_nCallReseed
(JNIEnv *env, jclass cls, jobject b, jobject s, jint c) {	
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    jbyte *sp = (*env)->GetDirectBufferAddress(env, s);
    ojr_call_reseed((ojr_generator *)ptr, (uint32_t *)sp, c);
}

JNIEXPORT void JNICALL Java_com_onejoker_randlib_Generator_nCallRefill
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
    ojr_call_refill((ojr_generator *)ptr);
}

JNIEXPORT jint JNICALL Java_com_onejoker_randlib_Generator_nNext16
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
	return ojr_next16((ojr_generator *)ptr);
}

JNIEXPORT jint JNICALL Java_com_onejoker_randlib_Generator_nNext32
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
	return ojr_next32((ojr_generator *)ptr);
}

JNIEXPORT jlong JNICALL Java_com_onejoker_randlib_Generator_nNext64
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
	return ojr_next64((ojr_generator *)ptr);
}

JNIEXPORT jdouble JNICALL Java_com_onejoker_randlib_Generator_nNextDouble
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
	return ojr_next_double((ojr_generator *)ptr);
}

JNIEXPORT jdouble JNICALL Java_com_onejoker_randlib_Generator_nNextSignedDouble
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
	return ojr_next_signed_double((ojr_generator *)ptr);
}

JNIEXPORT jdouble JNICALL Java_com_onejoker_randlib_Generator_nNextGaussian
(JNIEnv *env, jclass cls, jobject b) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
	return ojr_next_gaussian((ojr_generator *)ptr);
}

JNIEXPORT jint JNICALL Java_com_onejoker_randlib_Generator_nRand
(JNIEnv *env, jclass cls, jobject b, jint limit) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
	return ojr_rand((ojr_generator *)ptr, limit);
}

JNIEXPORT void JNICALL Java_com_onejoker_randlib_Generator_nDiscard
(JNIEnv *env, jclass cls, jobject b, jint count) {
    jbyte *ptr = (*env)->GetDirectBufferAddress(env, b);
	ojr_discard((ojr_generator *)ptr, count);
}
