#ifndef JNIDIRECT_LIBRARY_H
#define JNIDIRECT_LIBRARY_H


#if defined(_WIN32) || defined(__CYGWIN__)
#define windows true
#include "windows.h"
#elif defined(unix)
#define unix true
#include <unistd.h>
#elif defined(__APPLE__)
#define macosx true
#include <unistd.h>
#else
#error Unknown OS
#endif

#ifdef __cplusplus
extern "C" {
#endif


int JNIDirectPerform(void* generated, void* a1);

void* JNIDirectGenerateFuncWin64(void* a1, const void* targetF, int genSize);

__declspec(noinline)
void JNIDirectInit(void* targetF, void** generated_ptr);


#ifdef __cplusplus
}
#endif

#endif //JNIDIRECT_LIBRARY_H
