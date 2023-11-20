#include "JNIDirect.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#if WINDOWS
#include "windows.h"

#elif UNIX || MACOSX
//#include <unistd.h>
#include <sys/mman.h>
#include <sys/utsname.h>
typedef unsigned char byte;
#endif

#define DEBUG 0
#if DEBUG
#define printf(T) printf(T)
#else
#define printf(T)
#endif

enum arch {
    x86,
    x86_64,
    ARM,
    ARM64,
    UnknownArch,
    //
};

enum arch getCurrentArch() {
    static enum arch currentArch = -1;

	if(currentArch != -1) return currentArch;
#if WINDOWS
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	switch(info.wProcessorArchitecture) {
		case PROCESSOR_ARCHITECTURE_AMD64:
			return currentArch = x86_64;
		/*case PROCESSOR_ARCHITECTURE_INTEL:
			return currentArch = x86;
		case PROCESSOR_ARCHITECTURE_ARM:
			return currentArch = ARM;
		case PROCESSOR_ARCHITECTURE_ARM64:
			return currentArch = ARM64;*/
		default:
			return currentArch = UnknownArch;
	}
#elif UNIX
    struct utsname info;
    uname(&info);

    if(strcmp("x86_64", (const char *) &info.machine) == 0) {
        return currentArch = x86_64;
    }
    return currentArch = UnknownArch;
#endif
}


/**
 * @return
 * 0 - success<br>
 * 3 - invalid generated address<br>
 */
int JNIDirectPerformX86_64(void* generated, void* a1) {
	if(abs((int)((byte*)generated - (byte*)a1)) > 0xFFFFFFFF) {
		return EXIT_CODE_ERROR_OUT_OF_MEMORY;
	}
	uint32_t callOperand = ((byte*)generated - (byte*)a1);
	uint32_t* callOperandAddr = (uint32_t*)(((byte*)a1) - 5 + 1);

	*callOperandAddr = callOperand;
	
	printf("Replaced!\n");
	return EXIT_CODE_SUCCESS;
}

#if WIN64
void* allocRWX(void* a1, int size) {
    return VirtualAllocEx(
	    GetCurrentProcess(),
	    NULL,
	    size,
	    MEM_COMMIT,
	    PAGE_EXECUTE_READWRITE);
}
#elif UNIX
void* allocRWX(void* a1, int size) {
    return mmap(NULL,size,PROT_EXEC | PROT_READ | PROT_WRITE,MAP_SHARED | MAP_ANONYMOUS,0,0);
}
#endif
#define GEN_SIZE_X86_64 32


void* JNIDirectGenerateFuncX86_64(void* a1, const void* const targetF) {
    void* generated = allocRWX(a1,GEN_SIZE_X86_64);
    if(generated == NULL) return NULL;

    //insert function that call target function:
    byte* p = (byte*)generated;
	
    *p++ = 0x49; *p++ = 0xba; {
	    const void** pb = (const void**)p;
	    pb[0] = targetF;
    } p += sizeof(void*); // movabs $[targetF],%r10
    *p++ = 0x41; *p++ = 0xff; *p++ = 0xe2; //jmp *%r10

    return generated;
}

#define GEN_SIZE_ARM64 32
void* JNIDirectGenerateFuncARM64(void* a1, const void* targetF, int argc) {
    int genSize = GEN_SIZE_ARM64 + (argc << 1);
    void* generated = allocRWX(a1,genSize);
    //insert function that call target function:
    uint32_t* p = (uint32_t*)generated;

    //


    return 0;
}

void* __attribute__((noinline)) returnAddress() {
	return __builtin_return_address(2);
}

int __attribute__((noinline)) JNIDirectInit(void* const targetF, void** const generated_ptr, int argc) {
    if(getCurrentArch() == UnknownArch) return EXIT_CODE_ERROR_UNSUPPORTED_CPU;
	void* a1 = __builtin_return_address(2);
	byte* callAddr = (byte*)a1 - 5;
	if(*callAddr == (byte)0xe8/*call instruction*/) {
		if(*generated_ptr == NULL) {
			*generated_ptr = JNIDirectGenerateFuncX86_64(a1, targetF);
			if(*generated_ptr == 0)
				return EXIT_CODE_ERROR_OUT_OF_MEMORY;
		}
		return JNIDirectPerformX86_64(*generated_ptr, a1);
	}
    return EXIT_CODE_ERROR_NOT_COMPILED;
}
