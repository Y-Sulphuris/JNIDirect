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
		testI3(50,(short)51,52L);
	}
	private static native void test();
	private static native void testI2(int i1, int i2);
	private static native void testI3(int i1, short i2, long i3);
}
