# jnitest

Small JNI example that
 - calls C++ from Java
 - calls Java from C++
 - calls Java from native thread
 - create instances with parameters
 - return array of custom classes from C to Java
 - throw exception in native code
to show basic JNI usage.

Missing would be
 - Keeping link between C++ & Java Classes with field in Java
   that holds native pointer/handle
 - many other things...

Does currently only contain a build "script" for Visual Studio, but the files
run the same on other platforms if compiled correctly.

## Building & Testing
 - Fix paths in build.bat
 - Open the Visual Studio command prompt (watch to run the correct one, 32bit vs 64bit,
   matching your VM).
 - enter repo root directory
 - run build.bat

## Commands to remember
 - list functions + signatures of Java class
 
    javap -s -p {CLASS}

 - list object exports (Visual Studio)
   
    dumpbin.exe /exports {DLL PATH}

## References Used
 - https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/jniTOC.html
 - http://adamish.com/blog/archives/327
 - https://thebreakfastpost.com/2012/01/23/wrapping-a-c-library-with-jni-part-1/
 - http://www3.ntu.edu.sg/home/ehchua/programming/java/JavaNativeInterface.html

## Ideas/Stuff worth reading
 - RAII wrapper for JNI
   - http://engineering.socialpoint.es/cpp-wrapper-for-jni.html
 - Java Exception handling in JNI
   - http://stackoverflow.com/a/2125673
   - http://stackoverflow.com/a/15289742
   - http://www.developer.com/java/data/exception-handling-in-jni.html
 - Android JNI tips
   - https://developer.android.com/training/articles/perf-jni.html
 - JNI usage in an Eclipse plugin
   - https://www.eclipse.org/forums/index.php/t/97234/
 - IBM developerWorks - Java programming with JNI
   - https://www.ibm.com/developerworks/java/tutorials/j-jni/j-jni.html
 - Simple method for creating Java Objects in C++
   - http://stackoverflow.com/a/1961432

## Things to remember
 - Android has multiple classlaoders, so env->FindClass might fail depending
   on context where it is executed. Have a look at http://stackoverflow.com/a/16302771
   (also see comments), and don't forget to make the ClassLoader ref global...
 - When calling back into an Android app from a native thread you are on a non-UI
   thread. Trying to update will fail (just as if called from a Java worker thread).
   Use runOnUiThread to dispatch "offending" code to UI thread.
 - When using JNI in e.g. Tomcat and trying to reload you application, the reload
   will fail. Tomcat generates a new classloader for each application, but in one
   VM instance only one classloader may have loaded a particular native lib.
   See http://stackoverflow.com/a/13242602 for a workaround.