package test;

import org.openjdk.jmh.annotations.*;
import org.openjdk.jmh.infra.Blackhole;
import sun.misc.Unsafe;

import java.lang.reflect.Field;
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
	public static void main(String[] args) throws Exception{
		System.out.println("Hello world!" + unsafe.getLong(memory));
		Thread.sleep(1000);
		while(true) {
			methodToBeJitCompiled();
		}
	}
	private static void methodToBeJitCompiled() {
		System.out.println(getLong(memory));
	}
	private static native void test();
	private static native void testI2(int i1, int i2);
	private static native void testI3(int i1, short i2, long i3);
	private static native int testI1ReturnArg(int i1);
	private static native long testI1L1ReturnArg(int i1, long l1);
	private static native long retNoArgs();

	@Benchmark
	public void measureIWANTBENCHMARKSINTHISPROJECT(Blackhole bh) {
		getLong(memory);
		//testI1L1ReturnArg(5,7L);
		//testI3(5,(short)7,9L);
		//testI2(6,8);
	}

	static final Unsafe unsafe;
	static {
		try {
			Field f = Unsafe.class.getDeclaredField("theUnsafe");
			f.setAccessible(true);
			unsafe = (Unsafe)f.get(null);
		} catch (Throwable e) {
			throw new RuntimeException(e);
		}
	}
	static long memory = unsafe.allocateMemory(32);

	private static native long getLong(long ptr);

	@Benchmark
	public void measureGetLongDirect(Blackhole bh) {
		getLong(memory);
	}

	private static native long getLongJNI(long ptr);

	@Benchmark
	public void measureGetLongJNI(Blackhole bh) {
		getLongJNI(memory);
	}

	private static native long getLongCritical(long ptr);
	@Benchmark
	public void measureGetLongCritical(Blackhole bh) {
		getLongCritical(memory);
	}

	@Benchmark
	public long measureGetLongUnsafe(Blackhole bh) {
		return unsafe.getLong(memory);
	}
}
