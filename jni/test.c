#include "JNIDirect.h"

#include <jni.h>

void test() {
	printf("Hello JNIDirect\n");
}
void* test_generated = NULL;

void testI2(JNIDirectArgs jint i1, jint i2) {
	printf("JNIDirect args: %li\n",(i1+i2));
}
void* testI2_generated = NULL;

void testI3(JNIDirectArgs jint i1, jshort i2, jlong i3) {
	printf("JNIDirect args: %li, %i, %lli\n",i1,i2,i3);
}
void* testI3_generated = NULL;

JNIEXPORT void JNICALL Java_test_JNIDirectTest_test(JNIEnv *env, jclass cls) {
	printf("Hello JNI\n");
}
JNIEXPORT void JNICALL JavaCritical_test_JNIDirectTest_test() {
	JNIDirectInit(&test,&test_generated,0);
	printf("Hello JNICritical\n");
}

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
