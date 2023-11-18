package test;

public class JNIDirectTest {
	static {
		System.load("G:/Project/CLionProjects/JNIDirect/jni/cmake-build-debug/libJNIDirect.dll");
	}
	public static void main(String[] args) {
		while(true) {
			methodToBeJitCompiled();
		}
	}
	private static void methodToBeJitCompiled() {
		test();
	}
	private static native void test();
	private static native void testI2(int i1, int i2);
	private static native void testI3(int i1, short i2, long i3);
	private static native int testI1ReturnArg(int i1);
	private static native long testI1L1ReturnArg(int i1, long l1);
}
