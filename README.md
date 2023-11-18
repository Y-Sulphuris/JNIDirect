# JNIDirect
Direct JNI calls without overhead (HotSpot only)

**Project currently in early development stage (tested only on GCC win64, java 8, not stable)**<br>
ARM support will be added a bit later

This is a small library that allows you to direct invoke native methods from jit-compiled methods by replacing 
the instruction to call an overhead from a JIT compiled java method directly to calling the target function. 
You cannot use JNIEnv functions in these methods. 
Only primitives are allowed as function arguments and return types.

Unlike JNICritical, these methods do not stop the garbage collector, but they do not allow arrays to be passed as arguments and works faster than JNICritical.<br>
This is still intended for short methods calls.

Invocation native method with one dereference operation.

*Usage instruction will be added later...*