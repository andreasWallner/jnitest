package at.innovative_solutions.jnitest;

public class NativeStuff {
	static {
		System.loadLibrary("jniNativeExample");
	}
	
	public native void helloNative(int delayCnt, String[] strings);
	
	public static void main(String[] args) {
		new NativeStuff().helloNative(3, new String[]{"mad", "sad", "world"});
	}

	public void print(String s) {
		System.out.println(s);
	}
}