# JNIDirect
Direct JNI calls without overhead (HotSpot only)

**Project currently in early development stage (tested only on GCC win64, java 8, not stable)**<br>
ARM support will be added a bit later<br>
_Any contribution is welcome_.

This is a small library that allows you to direct invoke native methods from jit-compiled methods by replacing 
the instruction to call an overhead from a JIT compiled java method directly to calling the target function. 
You cannot use JNIEnv functions in these methods. 
Only primitives are allowed as function arguments and return types.

Unlike JNICritical, these methods do not stop the garbage collector, but they do not allow arrays to be passed as arguments and works faster than JNICritical.<br>
This is still intended for short methods calls.

Invocation native method with one dereference operation:
JNI:                8,773	ns/op
JNIDirect:          3,262	ns/op


Usage:

```java
//JNIDirectTest.java
package test;
public class JNIDirectTest {
    public static native void myfunc(int arg);
}
```

```c
/* library.c */

void myDirectFunc(jint arg) {
	//func implementation
}

//it will be points to runtime-generated function that calls myDirectFunc (only on 64 bit architecture, 32 bit doesn't need this)
void* myDirectFunc_generated = nullptr;

JNIEXPORT void JNICALL Java_test_JNIDirectTest_myfunc(JNIEnv*, jclass, jint arg) {
	JNIDirectInit((void*)&myDirectFunc,&myDirectFunc_generated);
	myDirectFunc(arg);
}
```
or:
```c
/* library.c */

void myDirectFunc(JNIDirectArgs jint arg) {
	//func implementation
}

//it will be points to runtime-generated function that calls myDirectFunc (only on 64 bit architecture, 32 bit doesn't need this)
void* myDirectFunc_generated = NULL;

JNIEXPORT void JNICALL JavaCritical_test_JNIDirectTest_myfunc(jint arg) {
	JNIDirectInit((void*)&myDirectFunc,&myDirectFunc_generated);
	myDirectFunc(JNIDirectAInvoke arg);
}
//fallback for other JVMs and non-jit compiled methods
JNIEXPORT void JNICALL Java_test_JNIDirectTest_myfunc(JNIEnv*, jclass, jint arg) {
	myDirectFunc(JNIDirectAInvoke arg);
}
```