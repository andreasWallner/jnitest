#include <jni.h>
#include "../at_innovative_solutions_jnitest_NativeStuff.h"
#include "../at_innovative_solutions_jnitest_NativeStuff_Wrapper.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>
#include <vector>
#include <memory>

#define JNI_VERSION JNI_VERSION_1_8

JavaVM *gJvm;
jclass CRuntimeError;

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *pjvm, void* /* reserved */) {
	// cache vm instance for usage in native threads
	gJvm = pjvm;
	JNIEnv *env;
	if(pjvm->GetEnv((void**)&env, JNI_VERSION))
		return -1; // failure, get us unloaded

	// cache object reference for performance here
	// do not forget to create global references for them
	jclass LocalCRuntimeError = env->FindClass("Ljava/lang/RuntimeException;");
	if(LocalCRuntimeError == nullptr)
		return -1;
	CRuntimeError = (jclass)env->NewGlobalRef(LocalCRuntimeError);

	return JNI_VERSION;
}

JNIEXPORT void JNICALL Java_at_innovative_1solutions_jnitest_NativeStuff_callJavaFromThread
  (JNIEnv * env, jobject self, jint delayCnt, jobjectArray arr) {
  	jclass CNativeStuff = env->GetObjectClass(self);
  	jmethodID NativeStuff_print = env->GetMethodID(CNativeStuff, "print", "(Ljava/lang/String;)V");
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

// if we are performance sensitive we should get the classes and method ids in OnLoad
// do not forget to create global refs for the classes, otherwise they will get collected
JNIEXPORT jobjectArray JNICALL Java_at_innovative_1solutions_jnitest_NativeStuff_returnArrayOfCustomClasses
  (JNIEnv *env, jobject ) {
	// FindClass throws an java.lang.NoClassDefFoundError, so we just have to return on error
	jclass CSubClass = env->FindClass("Lat/innovative_solutions/jnitest/NativeStuff$SubClass;");
	if (CSubClass == nullptr)
		return nullptr;

	// GetMethodID throws an java.lang.NoSuchMethodError, so again, just return on error
	jmethodID CSubClass_init = env->GetMethodID(CSubClass, "<init>", "(Ljava/lang/String;S)V");
	if (CSubClass_init == nullptr)
		return nullptr;

	// possible errors like OOM all throw, so again, only return
	jobjectArray result = env->NewObjectArray(2, CSubClass, nullptr);
	if (result == nullptr)
		return nullptr;

	std::vector<std::string> strings {"one", "two"};
	std::vector<short> shorts {1, 2};
	for(int i = 0; i < 2; ++i) {
		jstring s = env->NewStringUTF(strings[i].c_str());
		jobject element = env->NewObject(CSubClass, CSubClass_init, s, shorts[i]);
		env->SetObjectArrayElement(result, i, element);
	}

	return result;
}

JNIEXPORT void JNICALL Java_at_innovative_1solutions_jnitest_NativeStuff_throwAnException
  (JNIEnv *env, jobject) {
  	// do not forget to return from the middle of your code
  	// ThrowNew will only tell the runtime that you have thrown, it
  	// will not stop execution of native code
  	env->ThrowNew(CRuntimeError, "thrown from native code");
}

// this class will be owned by the Wrapper Java object
class WrappedNativeClass {
	int m_exp;
public:
	WrappedNativeClass(int exp) : m_exp(exp) {}
	int expInNative(int i) {
		int result = 1;
		for(int n = m_exp; n; --n)
			result *= i;
		return result;
	};
};

JNIEXPORT void JNICALL Java_at_innovative_1solutions_jnitest_NativeStuff_00024Wrapper_attachNativeObject
  (JNIEnv *env, jobject self, jint exp) {
  	// again... cache that class and IDs
  	jclass CWrapper = env->GetObjectClass(self);
  	if(CWrapper == nullptr)
  		return;
  	
  	jfieldID fNativeHandleID = env->GetFieldID(CWrapper, "fNativeHandle", "J");
  	if(fNativeHandleID == nullptr)
  		return;

	auto native = std::make_unique<WrappedNativeClass>(exp);
  	env->SetLongField(self, fNativeHandleID, (jlong)native.get());
  	native.release(); // Java object now owns the instance
}

JNIEXPORT jint JNICALL Java_at_innovative_1solutions_jnitest_NativeStuff_00024Wrapper_expInNative
  (JNIEnv *env, jobject self, jint i) {
	jclass CWrapper = env->GetObjectClass(self);
	if(CWrapper == nullptr)
		return 0;

	// yes, there really should be some helper methods to get/set the native handle...
	jfieldID fNativeHandleID = env->GetFieldID(CWrapper, "fNativeHandle", "J");
	if(fNativeHandleID == nullptr)
		return 0;

	WrappedNativeClass *native = (WrappedNativeClass*)env->GetLongField(self, fNativeHandleID);
	return native->expInNative(i);
}

JNIEXPORT void JNICALL Java_at_innovative_1solutions_jnitest_NativeStuff_00024Wrapper_dispose
  (JNIEnv *env, jobject self) {
  	std::cout << "disposing of native object" << std::endl;
	jclass CWrapper = env->GetObjectClass(self);
	if(CWrapper == nullptr)
		return;

	jfieldID fNativeHandleID = env->GetFieldID(CWrapper, "fNativeHandle", "J");
	if(fNativeHandleID == nullptr)
		return;

	// set field to 0 to prevent double deletes
	WrappedNativeClass *native = (WrappedNativeClass*)env->GetLongField(self, fNativeHandleID);
	env->SetLongField(self, fNativeHandleID, 0);
	delete native;
}