package test;

import org.openjdk.jmh.annotations.*;
import org.openjdk.jmh.infra.Blackhole;

import java.util.concurrent.TimeUnit;

@BenchmarkMode(Mode.AverageTime)
@OutputTimeUnit(TimeUnit.NANOSECONDS)
@Fork(value = 1)
@Warmup(iterations = 5)
@Measurement(iterations = 5)
public class JNIDirectTest {
	static {
		//windows:
		System.load("G:/Project/CLionProjects/JNIDirect/jni/cmake-build-debug/libJNIDirect.dll");
		//unix:
		//System.load("/mnt/hgfs/JNIDirect/jni/cmake-build-debug/libJNIDirect.so");
	}
	public static void main(String[] args) {
		System.out.println("Hello world!");
		while(true) {
			methodToBeJitCompiled();
		}
	}
	private static void methodToBeJitCompiled() {
		//test();
		//testI2(5,7);
		System.out.println(testI1L1ReturnArg(5,8));
	}
	private static native void test();
	private static native void testI2(int i1, int i2);
	private static native void testI3(int i1, short i2, long i3);
	private static native int testI1ReturnArg(int i1);
	private static native long testI1L1ReturnArg(int i1, long l1);
	private static native long retNoArgs();

	@Benchmark
	public void measureIWANTBENCHMARKSINTHISPROJECT(Blackhole bh) {
		testI1ReturnArg(5);
	}
}
