#ifndef JNIDIRECT_LIBRARY_H
#define JNIDIRECT_LIBRARY_H

#include <jni.h>


#if defined(_WIN32) || defined(__CYGWIN__)
#define WINDOWS 1
#elif defined(unix)
#define UNIX 1
#elif defined(__APPLE__)
#define MACOSX 1
#else
#error UnknownArch OS
#endif


#ifdef __cplusplus
extern "C" {
#endif

#define JNIDirectArgs jint JNIDirectArgs,
#define JNIDirectAInvoke 0,

int JNIDirectPerform(void* generated, void* a1);

void* __attribute__((noinline)) returnAddress();

/**
 * @param targetF Pointer to implementation of 'native' method
 * @param generated_ptr Pointer to store runtime-generated function to call target function
 * @param argc Amount of argument in native method
 * @return
 * 0 - success<br>
 * 1 - invoke instruction not found (java method is not JIT compiled yet, or this is not a HotSpot JVM)<br>
 * 2 - unsupported CPU architecture<br>
 * 3 - out of memory (cannot allocate memory for native method caller)
 */
int __attribute__((noinline)) JNIDirectInit(void* targetF, void** generated_ptr, int argc);

#define EXIT_CODE_SUCCESS 0
#define EXIT_CODE_ERROR_NOT_COMPILED 1
#define EXIT_CODE_ERROR_UNSUPPORTED_CPU 2
#define EXIT_CODE_ERROR_OUT_OF_MEMORY 3

typedef union {
	void* address;
	jlong value;
} jpointer;



#ifdef __cplusplus
}
#endif

#endif //JNIDIRECT_LIBRARY_H
