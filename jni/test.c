
#include "JNIDirect.h"


void test() {
	printf("Hello JNIDirect\n");
}
void* test_generated = NULL;

JNIEXPORT void JNICALL Java_test_JNIDirectTest_test(JNIEnv *env, jclass cls) {
	//printf("Hello JNI\n");
}

JNIEXPORT void JNICALL JavaCritical_test_JNIDirectTest_test() {
    int errc = JNIDirectInit(&test,&test_generated,0);
    if(errc != 0) printf("error while performing generated_ptr function: %08x",errc);
    printf("Hello JNICritical\n");
}

void testI2(JNIDirectArgs jint i1, jint i2) {
	printf("JNIDirect args: %i\n",(i1+i2));
}
void* testI2_generated = NULL;

void testI3(JNIDirectArgs jint i1, jshort i2, jlong i3) {
	printf("JNIDirect args: %li, %i, %lli\n",i1,i2,i3);
}
void* testI3_generated = NULL;


JNIEXPORT void JNICALL Java_test_JNIDirectTest_testI2(JNIEnv *env, jclass cls, jint i1, jint i2) {
	JNIDirectInit(&testI2,&testI2_generated,2);
	printf("JNI args: %li, %li\n",i1, i2);
}
JNIEXPORT void JNICALL Java_test_JNIDirectTest_testI3(JNIEnv *env, jclass cls, jint i1, jshort i2, jlong i3) {
	JNIDirectInit(&testI3,&testI3_generated,3);
	printf("Called from JNI:\n");
	testI3(JNIDirectAInvoke i1,i2,i3);
	//printf("JNI args: %li, %i, %lli\n",i1, i2, i3);
}


jint testI1ReturnArg(JNIDirectArgs jint i1) {
	return i1;
}

void* testI1ReturnArg_generated = NULL;

JNIEXPORT jint JNICALL Java_test_JNIDirectTest_testI1ReturnArg(JNIEnv *env, jclass cls, jint i1) {
	//JNIDirectInit(&testI1ReturnArg,&testI1ReturnArg_generated,1);
	//return testI1ReturnArg(JNIDirectAInvoke i1);
	//printf("JNI args: %li, %i, %lli\n",i1, i2, i3);
	return i1;
}



jlong testI1L1ReturnArg(JNIDirectArgs jint i1, jlong l1) {
	return l1;
}

void* testI1L1ReturnArg_generated = NULL;

JNIEXPORT jlong JNICALL Java_test_JNIDirectTest_testI1L1ReturnArg(JNIEnv *env, jclass cls, jint i1, jlong l1) {
	return i1;
	return testI1L1ReturnArg(JNIDirectAInvoke i1, l1);
}
JNIEXPORT jlong JNICALL JavaCritical_test_JNIDirectTest_testI1L1ReturnArg(jint i1, jlong l1) {
	JNIDirectInit(&testI1L1ReturnArg,&testI1L1ReturnArg_generated,2);
	return i1;
	return testI1L1ReturnArg(JNIDirectAInvoke i1, l1);
}


jlong retNoArgs() {
	return 52L;
}

void* retNoArgs_generated = NULL;

JNIEXPORT jlong JNICALL Java_test_JNIDirectTest_retNoArgs(JNIEnv *env, jclass cls) {
	JNIDirectInit(&retNoArgs,&retNoArgs_generated,0);
	return retNoArgs();
}

