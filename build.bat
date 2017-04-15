set JDK_PATH=c:/Program Files/Java/jdk1.8.0_121
pushd java
"%JDK_PATH%/bin/javac.exe" at/innovative_solutions/jnitest/NativeStuff.java
if errorlevel 1 goto onerror
"%JDK_PATH%/bin/javah.exe" -jni at.innovative_solutions.jnitest.NativeStuff
if errorlevel 1 goto onerror
popd

pushd c
cl /W4 /EHsc /D_USRDLL /D_WINDLL /IC:\Programme\Java\jdk1.8.0_121\include /IC:\Programme\Java\jdk1.8.0_121\include\win32 nativestuff.cpp /MT /link /DLL /OUT:jniNativeExample.dll
if errorlevel 1 goto onerror
popd

java -Djava.library.path=./c -classpath ./java at.innovative_solutions.jnitest.NativeStuff

goto end

:onerror
echo ERROR

:end