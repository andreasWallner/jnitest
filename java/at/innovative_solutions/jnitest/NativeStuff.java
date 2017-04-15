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

	static {
		System.loadLibrary("jniNativeExample");
	}

	public native void helloNative(int delayCnt, String[] strings);
	public native SubClass[] returnArrayOfCustomClasses();
	public native void throwAnException();
	
	public static void main(String[] args) {
		NativeStuff n = new NativeStuff();

		n.helloNative(3, new String[]{"mad", "sad", "world"});

		SubClass[] arr = n.returnArrayOfCustomClasses();
		for(SubClass e : arr) {
			System.out.println(e.toString());
		}

		try {
			n.throwAnException();
		} catch(Exception e) {
			System.out.println(e.getMessage());
		}
	}

	public void print(String s) {
		System.out.println(s);
	}
}