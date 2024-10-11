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

#define DEBUG 1
#if DEBUG
#define printf(T...) printf(T)
#else
#define printf(T)
#endif


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
#define GEN_SIZE_X86_64 64

#if WIN64
int roundup(int x, int y) {
	return ((x+y-1) & ~(y-1));
}
void* currentRWXBlock = NULL;
int sfp = 0; // function_stack_pointer

void* allocRWX(int size) {
	if(currentRWXBlock == NULL || sfp > 40000) {
		currentRWXBlock = VirtualAllocEx(
			GetCurrentProcess(),
			NULL,
			40000,
			MEM_COMMIT,
			PAGE_EXECUTE_READWRITE);
		sfp = 0;
	}
	void* allocated = (char*)currentRWXBlock + sfp;
	size = roundup(size, 8);
	sfp += size;
	return allocated;
}

#elif UNIX
void* allocRWX(void* a1, int size) {
    return mmap(NULL,size,PROT_EXEC | PROT_READ | PROT_WRITE,MAP_SHARED | MAP_ANONYMOUS,0,0);
}
#endif

/**
 * @return
 * 0 - success<br>
 * 2 - unsupported CPU<br>
 * 3 - invalid generated address<br>
 */
int JNIDirectPerform(void* generated, void* a1) {
	switch(getCurrentArch()) {
		case x86_64:
			return JNIDirectPerformX86_64(generated,a1);
		case ARM64:
		case x86:
		case ARM:
		case UnknownArch:
			return EXIT_CODE_ERROR_UNSUPPORTED_CPU;
	}
}



void* JNIDirectGenerateBridgeX86_64(void* a1, const void* const targetF, int argc) {
    void* generated = allocRWX(GEN_SIZE_X86_64);
    if(generated == NULL) return NULL;

    //insert function that calls target function:
    byte* p = (byte*)generated;
    
    if(argc > 1) {
	    *p++ = 0x48; *p++ = 0x83; *p++ = 0xec; *p++ = (argc << 3) + 8; //sub $(argc << 3),%rsp
	    
	    
	    *p++ = 0x49; *p++ = 0xba; // movabs $[targetF],%r10
	    {
		    const void** pb = (const void**)p;
		    pb[0] = targetF;
	    }
	    p += sizeof(void*);
	    
	    *p++ = 0x41; *p++ = 0xff; *p++ = 0xd2; // call *%r10
	    
	    *p++ = 0x90; // nop
	    //*p++ = 0xc5; *p++ = 0xf8; *p++ = 0x77; // vzeroupper
	    
	    *p++ = 0x48; *p++ = 0x83; *p++ = 0xc4; *p++ = (argc << 3) + 0x8; //add    $(argc << 3),%rsp
	    *p++ = 0xc3; //ret
    } else {
	    //*p++ = 0x48; *p++ = 0x83; *p++ = 0xec; *p++ = 0x20; //sub $0x20,%rsp
	    
	    *p++ = 0x49; *p++ = 0xba; {
		    const void** pb = (const void**)p;
		    pb[0] = targetF;
	    } p += sizeof(void*); // movabs $[targetF],%r10
	    *p++ = 0x90; *p++ = 0x90;
	    *p++ = 0x41; *p++ = 0xff; *p++ = 0xe2; //jmp *%r10
    }
    
    return generated;
}

#define GEN_SIZE_ARM64 32
void* JNIDirectGenerateBridgeARM64(void* a1, const void* targetF, int argc) {
    int genSize = GEN_SIZE_ARM64 + (argc << 1);
    void* generated = allocRWX(genSize);
    if(generated == NULL) return NULL;
    //insert function that call target function:
    uint32_t* p = (uint32_t*)generated;

    //todo


    return NULL;
}


void* JNIDirectGenerateBridge(void* a1, const void* targetF, int argc) {
	switch (getCurrentArch()) {
		case x86_64:
			return JNIDirectGenerateBridgeX86_64(a1,targetF,argc);
		case ARM64:
			return JNIDirectGenerateBridgeARM64(a1,targetF,argc);
		case x86:
		case ARM:
		case UnknownArch:
			return NULL;
	}
}

void* __attribute__((noinline)) returnAddress() {
	return __builtin_return_address(2);
}

int __attribute__((noinline)) JNIDirectInit(void* const targetF, void** const bridge_ptr, int argc) {
    if(getCurrentArch() == UnknownArch) return EXIT_CODE_ERROR_UNSUPPORTED_CPU;
	void* a1 = __builtin_return_address(2);
	byte* callAddr = (byte*)a1 - 5;
	if(*callAddr == (byte)0xe8/*call instruction*/) {
		if(*bridge_ptr == NULL) {
			*bridge_ptr = JNIDirectGenerateBridge(a1, targetF, argc);
			if(*bridge_ptr == NULL)
				return EXIT_CODE_ERROR_OUT_OF_MEMORY;
		}
		return JNIDirectPerform(*bridge_ptr, a1);
	}
    return EXIT_CODE_ERROR_NOT_COMPILED;
}


int JNIDirectInitIntrinsic(void *(*generate)(enum arch, void *), void **generated_ptr, void *args, void* fallback, int fallback_argc) {
	if(getCurrentArch() == UnknownArch) return EXIT_CODE_ERROR_UNSUPPORTED_CPU;
	void* a1 = __builtin_return_address(2);
	byte* callAddr = (byte*)a1 - 5;
	if(*callAddr == (byte)0xe8/*call instruction*/) {
		if(*generated_ptr == NULL) {
			*generated_ptr = generate(getCurrentArch(), args);
			if(*generated_ptr == NULL) {
				*generated_ptr = JNIDirectGenerateBridge(a1, fallback, fallback_argc);
				if(*generated_ptr == NULL)
					return EXIT_CODE_ERROR_OUT_OF_MEMORY;
			}
		}
		return JNIDirectPerform(*generated_ptr, a1);
	}
	return EXIT_CODE_ERROR_NOT_COMPILED;
}
