package at.innovative_solutions.jnitest;

public class NativeStuff {
	public static class SubClass {
		public final String fTheString;
		public final short fTheShort;
		public SubClass(String theString, short theShort) {
			fTheString = theString;
			fTheShort = theShort;
		}
		@Override
		public String toString() {
			return "SubClass(" + fTheString + ", " + fTheShort + ")";
		}
	}

	public static class Wrapper {
		private long fNativeHandle;

		Wrapper(int exponent) {
			attachNativeObject(exponent);
		}
		private native void attachNativeObject(int exponent);
		public native int expInNative(int i);
		public native void dispose();

		public void finalize() {
			if(fNativeHandle != 0)
				System.err.println("WARNING: Wrapper objects left un-disposed");
		}
	}

	static {
		System.loadLibrary("jniNativeExample");
	}

	public native void callJavaFromThread(int delayCnt, String[] strings);
	public native SubClass[] returnArrayOfCustomClasses();
	public native void throwAnException();
	
	public static void main(String[] args) {
		NativeStuff n = new NativeStuff();

		n.callJavaFromThread(3, new String[]{"mad", "sad", "world"});

		SubClass[] arr = n.returnArrayOfCustomClasses();
		for(SubClass e : arr) {
			System.out.println(e.toString());
		}

		try {
			n.throwAnException();
		} catch(Exception e) {
			System.out.println(e.getMessage());
		}

		Wrapper w = new Wrapper(3);
		System.out.println(w.expInNative(3));
		w.dispose();
	}

	public void print(String s) {
		System.out.println(s);
	}
}