//
// Created by Jianan on 11/4/2016.
//

#include <jni.h>
#include <string>

extern "C"
jstring Java_edu_asu_msrs_artcelerationlibrary_TransformService_stringFromJNI(
        JNIEnv *env,
        jobject ) {

    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

