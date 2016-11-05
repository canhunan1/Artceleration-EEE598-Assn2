//
// Created by Jianan on 11/4/2016.
//

#include <jni.h>
#include <string>
extern "C"
JNIEXPORT jstring JNICALL
Java_edu_asu_msrs_artcelerationlibrary_TestService_myStringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    jstring hello = (jstring) "ddddd";
    return hello;
}

JNIEXPORT jint JNICALL
Java_edu_asu_msrs_artcelerationlibrary_TestService_JNI_1OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    // Get jclass with env->FindClass.
    env->FindClass("");
    // Register methods with env->RegisterNatives.
    //env->RegisterNatives();

    return JNI_VERSION_1_6;
}
