# JNIDirect
Direct native methods calls without JNI overhead (HotSpot only)


**Project currently in early development stage (tested only on x86_64)**<br>
Required: `java 8+`<br>
ARM support will be added a bit later<br>
_Any contribution is welcome._

This small library allows you to direct invoke native methods from jit-compiled java methods by replacing 
the instruction to call a "JNI overhead" from a JIT compiled java method directly to calling the target function. 
You cannot use JNIEnv functions in these methods. 
Only primitives are allowed as function arguments and return types.

Unlike JNICritical, these methods do not transferred thread to state "native" and works faster, but they do not allow arrays to be passed as arguments.<br>
This is still intended for short methods calls.

```
Invocation native method (takes 1 int argument and returns it):
JNI:                                    6,767 ± 0,267  ns/op
JNIDirect:                              2,522 ± 1,051  ns/op

Long pointer dereference ( getLong(ptr) ):
JNI:                                    7,441 ± 0,855  ns/op
JNICritical:                            8,861 ± 4,465  ns/op
JNIDirect:                              2,237 ± 0,135  ns/op
Unsafe (HotSpot intrinsic):             3,220 ± 1,100  ns/op   
JNIDirectIntrinsic (runtime-generated): 1,438 ± 0,062  ns/op


Platform: Windows 10 x64, Intel(R) Core(TM) i5-9300H CPU @ 2.40GHz
```

## How to use:

To make native method _direct:_
1. Move the implementation of the method into a separate 
function without a specific name. <br>
Remove the JNIEnv and jclass arguments.<br>
If function takes any other arguments, put macro `JNIDirectArgs` before them.
2. Declare a non-const global field `void*` with the same name as new function and add postfix `_bridge`. Set value to NULL.<br>
This pointer will be used to store address to generated machine code which jumps from java method to your function on 64bit architectures.
3. Make old JNI function (function with `JNIEXPORT & JNICALL` which you used before) invoke this new function.<br>
If function takes any args, put macro `JNIDirectAInvoke` before it.<br>
This is a fallback for non-support JVM's and non-compiled methods.
4. In old JNI function before invocation a new one, add call to `JNIDirectInit`:<br>
`JNIDirectInit((void*)&[your new function name],&[your new function name]_bridge,[function arguments amount]);`
    1. For better performance you may put it in [JavaCritical](https://bugs.openjdk.org/browse/JDK-7013347) function to avoid burdening non-compiled methods with attempts to load a JNIDirect call. But make sure that target JVM will support JNICritical, if it supports JNIDirect.

Notice: if you use C++, all this functions have to be in `extern "C" {}` block.

### Example:

```java
//JNIDirectTest.java
package test;
public class JNIDirectTest {
    public static native long myfunc(int arg);
}
```

```c
/* library.c */

jlong myDirectFunc(JNIDirectArgs jint arg) {
	//func implementation
}

//it will be points to runtime-generated function that calls myDirectFunc (only on 64 bit architecture, 32 bit doesn't need this)
void* myDirectFunc_bridge = NULL;

//called once when the method has just been recompiled by JIT
JNIEXPORT jlong JNICALL JavaCritical_test_JNIDirectTest_myfunc(jint arg) {
	JNIDirectInit((void*)&myDirectFunc,&myDirectFunc_bridge,1);
	return myDirectFunc(JNIDirectAInvoke arg);
}
//fallback for other JVMs and non-jit compiled methods
JNIEXPORT jlong JNICALL Java_test_JNIDirectTest_myfunc(JNIEnv* env, jclass caller, jint arg) {
	return myDirectFunc(JNIDirectAInvoke arg,1);
}
```


_There is minimum security checks for better performance, so you have to be very careful_

This library does not provide ready-made JNI functions or any Java API.
