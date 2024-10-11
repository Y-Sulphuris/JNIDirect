
#include <stdint.h>
#include "JNIDirect.h"


void test() {
	printf("Hello JNIDirect\n");
}
void* test_bridge = NULL;

JNIEXPORT void JNICALL Java_test_JNIDirectTest_test(JNIEnv *env, jclass cls) {
	//printf("Hello JNI\n");
}

JNIEXPORT void JNICALL JavaCritical_test_JNIDirectTest_test() {
    int errc = JNIDirectInit(&test,&test_bridge,0);
    if(errc != 0) printf("error while performing bridge_ptr function: %08x",errc);
    printf("Hello JNICritical\n");
}

void testI2(JNIDirectArgs jint i1, jint i2) {
	//printf("JNIDirect args: %li (%li + %li)\n",(i1+i2),i1,i2);
}
void* testI2_bridge = NULL;

void testI3(JNIDirectArgs jint i1, jshort i2, jlong i3) {
	printf("JNIDirect args: %li, %i, %lli\n",i1,i2,i3);
}
void* testI3_bridge = NULL;


JNIEXPORT void JNICALL Java_test_JNIDirectTest_testI2(JNIEnv *env, jclass cls, jint i1, jint i2) {
	JNIDirectInit(&testI2,&testI2_bridge,2);
	printf("JNI args: %li, %li\n",i1, i2);
}
JNIEXPORT void JNICALL Java_test_JNIDirectTest_testI3(JNIEnv *env, jclass cls, jint i1, jshort i2, jlong i3) {
	JNIDirectInit(&testI3,&testI3_bridge,3);
	printf("Called from JNI:\n");
	testI3(JNIDirectAInvoke i1,i2,i3);
	//printf("JNI args: %li, %i, %lli\n",i1, i2, i3);
}


jint testI1ReturnArg(JNIDirectArgs jint i1) {
	return i1;
}

void* testI1ReturnArg_bridge = NULL;

JNIEXPORT jint JNICALL Java_test_JNIDirectTest_testI1ReturnArg(JNIEnv *env, jclass cls, jint i1) {
	JNIDirectInit(&testI1ReturnArg,&testI1ReturnArg_bridge,1);
	return testI1ReturnArg(JNIDirectAInvoke i1);
	//printf("JNI args: %li, %i, %lli\n",i1, i2, i3);
}



jlong testI1L1ReturnArg(JNIDirectArgs jint i1, jlong l1) {
	return l1;
}

void* testI1L1ReturnArg_bridge = NULL;

JNIEXPORT jlong JNICALL Java_test_JNIDirectTest_testI1L1ReturnArg(JNIEnv *env, jclass cls, jint i1, jlong l1) {
	return i1;
	return testI1L1ReturnArg(JNIDirectAInvoke i1, l1);
}
JNIEXPORT jlong JNICALL JavaCritical_test_JNIDirectTest_testI1L1ReturnArg(jint i1, jlong l1) {
	JNIDirectInit(&testI1L1ReturnArg,&testI1L1ReturnArg_bridge,2);
	return i1;
	return testI1L1ReturnArg(JNIDirectAInvoke i1, l1);
}


jlong retNoArgs() {
	return 52L;
}

void* retNoArgs_bridge = NULL;

JNIEXPORT jlong JNICALL Java_test_JNIDirectTest_retNoArgs(JNIEnv *env, jclass cls) {
	JNIDirectInit(&retNoArgs,&retNoArgs_bridge,0);
	return retNoArgs();
}

jlong getLongFallback(const jlong* ptr) {
	return *ptr;
}

void* getLong_generated = NULL;

void* generateGetLong(enum arch cpu, void* args_ignored) {
	switch (cpu) {
		case x86: {
			break;
		}
		case x86_64: {
			void* const generated = allocRWX(8);
			if(generated == NULL) return NULL;
			uint8_t* p = (uint8_t*)generated;
			
			*p++ = 0x48; *p++ = 0x8B; *p++ = 0x02; //mov rax, [rdx]
			*p++ = 0xc3; //ret
			
			return generated;
		}
		case ARM: {
			break;
		}
		case ARM64: {
			break;
		}
		default:
			return NULL;
	}
	return NULL;
}


JNIEXPORT jlong JNICALL Java_test_JNIDirectTest_getLongNoCritical(JNIEnv *env, jclass cls, jlong ptr) {
	JNIDirectInitIntrinsic(generateGetLong, &getLong_generated, NULL, &getLongFallback, 1);
	return *(jlong*)ptr;
}

JNIEXPORT jlong JNICALL Java_test_JNIDirectTest_getLong(JNIEnv *env, jclass cls, jlong ptr) {
	return *(jlong*)ptr;
}
JNIEXPORT jlong JNICALL JavaCritical_test_JNIDirectTest_getLong(jlong ptr) {
	JNIDirectInitIntrinsic(generateGetLong, &getLong_generated, NULL, &getLongFallback, 1);
	return *(jlong*)ptr;
}


JNIEXPORT jlong JNICALL Java_test_JNIDirectTest_getLongJNI(JNIEnv *env, jclass cls, jlong ptr) {
	return *(jlong*)ptr;
}


JNIEXPORT jlong JNICALL Java_test_JNIDirectTest_getLongCritical(JNIEnv *env, jclass cls, jlong ptr) {
	return *(jlong*)ptr;
}
JNIEXPORT jlong JNICALL JavaCritical_test_JNIDirectTest_getLongCritical(jlong ptr) {
	return *(jlong*)ptr;
}