#include <jni.h>
#include "../java/at_innovative_solutions_jnitest_NativeStuff.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

#define JNI_VERSION JNI_VERSION_1_8

JavaVM *gJvm;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *pjvm, void* /* reserved */) {
	// cache vm instance for usage in native threads
	gJvm = pjvm;
	JNIEnv env;
	if(pjvm->GetEnv((void**)&env, JNI_VERSION))
		return -1; // failure, get us unloaded

	// cache object reference for performance here
	// do not forget to create global references for them

	return JNI_VERSION;
}

JNIEXPORT void JNICALL Java_at_innovative_1solutions_jnitest_NativeStuff_helloNative
  (JNIEnv * env, jobject self, jint delayCnt, jobjectArray arr) {
  	jclass CNativeStuff = env->GetObjectClass(self);
  	jmethodID NativeStuff_print = env->GetMethodID(c, "print", "(Ljava/lang/String;)V");
  	if(NativeStuff_print == nullptr)
  		return; // exception gets thrown into JVM, but should never happen
	
	jint arrLen = env->GetArrayLength(arr);
  	for(int i = 0; i < arrLen; i++) {
  		std::ostringstream ss;

  		jstring s = (jstring)env->GetObjectArrayElement(arr, i);
		const char *str = env->GetStringUTFChars(s, 0);
		
		ss << "Hello " << str;
		std::string mys = ss.str();
		jstring mystr = env->NewStringUTF(mys.c_str());

		env->CallObjectMethod(self, NativeStuff_print, (jobject)mystr);

		env->DeleteLocalRef((jobject)mystr);
		env->ReleaseStringUTFChars(s, str);

		// for exception handling see
		// http://www.developer.com/java/data/exception-handling-in-jni.html
		if(env->ExceptionCheck() == JNI_TRUE) {
			env->ExceptionDescribe();
			return;
		}
	}

	std::thread t1([&](jobject me) {
		JNIEnv *env;

		// If we are unsure if the thead is already attached we
		// have to use GetEnv to check and attach if not already
		// done so
		// 
		// switch(gJvm->GetEnv((void**)&env, JNI_VERSION)) {
		//   case JNI_EDETACHED:
		//     // not attached, do so (and do not forget to
		//     // detach again afterwards (RAII...))
		//     break;
		//   case JNI_EVERSION:
		//     // JNI version not support, fail... (and print to stderr?)
		//     break;
		//   case JNI_OK:
		//     // env valid now
		//     break;
		// }
		if(gJvm->AttachCurrentThread((void**)&env, nullptr) != JNI_OK)
			return;

		for(int i = 0; i < delayCnt; i++) {
			std::string hello {"Hello after " + std::to_string(i+1) + "s"};
			std::this_thread::sleep_for(std::chrono::seconds(1));
			jstring s = env->NewStringUTF(hello.c_str());
			
			// NativeStuff_print may be reused, no making it global needed as
			// it is a method ID
			env->CallObjectMethod(me, NativeStuff_print, (jobject)s); 
			// skipped exception check as we do not use any return value
			// here, so no need to exit early, will get thrown into VM
			// anyhow

			env->DeleteLocalRef((jobject)s);
		}
		env->DeleteGlobalRef(me);
		// attach/detach is costly, so you might want to
		// e.g. maintain a thread pool of attached threads
		// or use long running thread etc.
		gJvm->DetachCurrentThread(); 
	}, env->NewGlobalRef(self)); // need global ref of self for native thread

	t1.join(); // make it easy for the example... we could e.g. also detach thread
}