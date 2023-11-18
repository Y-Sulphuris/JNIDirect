#include "JNIDirect.h"

#include <stdio.h>
#include <stdint.h>


int JNIDirectPerform(void* generated, void* a1) {
	uint64_t callOperand = ((byte*)generated - (byte*)a1);
	uint32_t* callOperandAddr = (uint32_t*)(((byte*)a1) - 5 + 1);
	if(callOperand != (uint32_t)callOperand) {
		return 1;
	}
	
	*callOperandAddr = callOperand;
	
	printf("Replaced!\n");
	return 0;
}

void* JNIDirectGenerateFuncWin64(void* a1, const void* const targetF, const int genSize) {
	void* generated = VirtualAllocEx(
		GetCurrentProcess(),
		NULL,
		genSize,
		MEM_COMMIT,
		PAGE_EXECUTE_READWRITE);
	while ((byte*)generated < (byte*)a1) {
		printf("err: generated < a1. Trying to realloc...");
		VirtualFreeEx(GetCurrentProcess(),generated,genSize,MEM_RELEASE);
		generated = (char*)VirtualAllocEx(
			GetCurrentProcess(),
			NULL,
			genSize,
			MEM_COMMIT,
			PAGE_EXECUTE_READWRITE);
	}
	
	//insert function that call target function:
	byte* p = (byte*)generated;
	
	*p++ = 0x41; *p++ = 0x52;// push r10
	
	*p++ = 0x49; *p++ = 0xba; // movabs $[targetF],%r10
	{
		const void** pb = (const void**)p;
		pb[0] = targetF;
	}
	p += sizeof(void*);
	
	*p++ = 0x41; *p++ = 0xff; *p++ = 0xd2; // call *%r10
	
	*p++ = 0x41; *p++ = 0x5a;// pop r10
	*p++ = 0xC3; // ret
	
	return generated;
}



__declspec(noinline)
void JNIDirectInit(void* const targetF, void** const generated_ptr) {
	void* a1 = __builtin_return_address(2);
	byte* callAddr = (byte*)a1 - 5;
	if(*callAddr == (byte)0xe8/*call instruction*/) {
		if(*generated_ptr == NULL) {
			*generated_ptr = JNIDirectGenerateFuncWin64(a1, targetF, 32);
		}
		int errc = JNIDirectPerform(*generated_ptr, a1);
		if(errc != 0) printf("error while performing generated_ptr function: %08x",errc);
	}
}

