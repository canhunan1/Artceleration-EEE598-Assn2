/*
//
// Created by Jianan on 11/4/2016.
//

#include <jni.h>
#include <string>
#include <android/log.h>
#include <stdio.h>
#include <android/bitmap.h>
#include <cstring>

#define  LOG_TAG    "DEBUG"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

//extern "C"
extern "C"
{
JNIEXPORT jobject JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniStoreBitmapData(JNIEnv * env, jobject obj, jobject bitmap);
JNIEXPORT jobject JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniGetBitmapFromStoredBitmapData(JNIEnv * env, jobject obj, jobject handle);
JNIEXPORT void JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniFreeBitmapData(JNIEnv * env, jobject obj, jobject handle);
JNIEXPORT void JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniRotateBitmapCcw90(JNIEnv * env, jobject obj, jobject handle);
JNIEXPORT void JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniCropBitmap(JNIEnv * env, jobject obj, jobject handle, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);
}




JNIEXPORT jstring JNICALL
Java_edu_asu_msrs_artcelerationlibrary_TransformService_myStringFromJNI(
        JNIEnv *env,
        jobject */
/* this *//*
) {
    jstring hello = (jstring) "ddddd";
    return hello;
}

*/
/*JNIEXPORT jint JNICALL
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
}*//*

// use byte array to store the bitmap.
class JniBitmap
{
    public:
        uint32_t* pixels;
        AndroidBitmapInfo bitmapInfo;
        JniBitmap()
        {
            pixels = NULL;
        }
};
JNIEXPORT void JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniRotateBitmapCcw90(JNIEnv * env, jobject obj, jobject handle)
{
    JniBitmap* jniBitmap = (JniBitmap*) env->GetDirectBufferAddress(handle);
    if (jniBitmap->pixels == NULL)
        return;
    uint32_t* previousData = jniBitmap->pixels;
    AndroidBitmapInfo bitmapInfo = jniBitmap->bitmapInfo;
    uint32_t* newBitmapPixels = new uint32_t[bitmapInfo.height * bitmapInfo.width];
    int whereToPut = 0;
    // A.D D.C
    // ...>...
    // B.C A.B
    for (int x = bitmapInfo.width - 1; x >= 0; --x)
        for (int y = 0; y < bitmapInfo.height; ++y)
        {
            uint32_t pixel = previousData[bitmapInfo.width * y + x];
            newBitmapPixels[whereToPut++] = pixel;
        }
    delete[] previousData;
    jniBitmap->pixels = newBitmapPixels;
    uint32_t temp = bitmapInfo.width;
    bitmapInfo.width = bitmapInfo.height;
    bitmapInfo.height = temp;
}
*/
/**free bitmap*//*
  //
JNIEXPORT void JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniFreeBitmapData(JNIEnv * env, jobject obj, jobject handle)
{
    JniBitmap* jniBitmap = (JniBitmap*) env->GetDirectBufferAddress(handle);
    if (jniBitmap->pixels == NULL)
        return;
    delete[] jniBitmap->pixels;
    jniBitmap->pixels = NULL;
    delete jniBitmap;
}

*/
/**restore java bitmap (from JNI data)*//*
  //
JNIEXPORT jobject JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniGetBitmapFromStoredBitmapData(JNIEnv * env, jobject obj, jobject handle)
{
    JniBitmap* jniBitmap = (JniBitmap*) env->GetDirectBufferAddress(handle);
    if (jniBitmap->pixels == NULL)
    {
        LOGD("no bitmap data was stored. returning null...");
        return NULL;
    }
    //
    //creating a new bitmap to put the pixels into it - using Bitmap Bitmap.createBitmap (int width, int height, Bitmap.Config config) :
    //
    //LOGD("creating new bitmap...");
    jclass bitmapCls = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapFunction = env->GetStaticMethodID(bitmapCls, "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    jstring configName = env->NewStringUTF("ARGB_8888");
    jclass bitmapConfigClass = env->FindClass("android/graphics/Bitmap$Config");
    jmethodID valueOfBitmapConfigFunction = env->GetStaticMethodID(bitmapConfigClass, "valueOf", "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    jobject bitmapConfig = env->CallStaticObjectMethod(bitmapConfigClass, valueOfBitmapConfigFunction, configName);
    jobject newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction, jniBitmap->bitmapInfo.width, jniBitmap->bitmapInfo.height, bitmapConfig);
    //
    // putting the pixels into the new bitmap:
    //
    int ret;
    void* bitmapPixels;
    if ((ret = AndroidBitmap_lockPixels(env, newBitmap, &bitmapPixels)) < 0)
    {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return NULL;
    }
    uint32_t* newBitmapPixels = (uint32_t*) bitmapPixels;
    int pixelsCount = jniBitmap->bitmapInfo.height * jniBitmap->bitmapInfo.width;
    memcpy(newBitmapPixels, jniBitmap->pixels, sizeof(uint32_t) * pixelsCount);
    AndroidBitmap_unlockPixels(env, newBitmap);
    //LOGD("returning the new bitmap");
    return newBitmap;
}
*/
/**store java bitmap as JNI data*//*
  //
JNIEXPORT jobject JNICALL Java_com_jni_bitmap_1operations_JniBitmapHolder_jniStoreBitmapData(JNIEnv * env, jobject obj, jobject bitmap)
{
    AndroidBitmapInfo bitmapInfo;
    uint32_t* pixels = NULL;
    int ret;
    */
/*
     * write the bitmapinfo struct
     * typedef struct{
        uint32_t width;
        uint32_t height;
        uint32_t stride;
        uint32_t format;
        uint32_t flags;
    }AndroidBitmapInfo;
     * *//*

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo)) < 0)
    {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return NULL;
    }
    LOGD("width:%d height:%d stride:%d", bitmapInfo.width, bitmapInfo.height, bitmapInfo.stride);
    // here we use android ARGB_8888
    if (bitmapInfo.format != ANDROID_BITMAP_FORMAT_RGBA_8888)
    {
        LOGE("Bitmap format is not RGBA_8888!");
        return NULL;
    }
    //
    //read pixels of bitmap into native memory :
    //
    //LOGD("reading bitmap pixels...");
    void* bitmapPixels;
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) < 0)
    {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return NULL;
    }
    uint32_t* src = (uint32_t*) bitmapPixels;
    pixels = new uint32_t[bitmapInfo.height * bitmapInfo.width];
    int pixelsCount = bitmapInfo.height * bitmapInfo.width;
    memcpy(pixels, src, sizeof(uint32_t) * pixelsCount);
    AndroidBitmap_unlockPixels(env, bitmap);
    JniBitmap *jniBitmap = new JniBitmap();
    jniBitmap->bitmapInfo = bitmapInfo;
    jniBitmap->pixels = pixels;
    return env->NewDirectByteBuffer(jniBitmap, 0);
}
*/

#include <jni.h>
#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <android/bitmap.h>
#include <cstring>
#include <unistd.h>

#define  LOG_TAG    "DEBUG"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C"
{
JNIEXPORT jstring JNICALL
        Java_edu_asu_msrs_artcelerationlibrary_TransformService_myStringFromJNI(
        JNIEnv *env,
        jobject
);
}

JNIEXPORT jstring JNICALL
        Java_edu_asu_msrs_artcelerationlibrary_TransformService_myStringFromJNI(
        JNIEnv *env,
        jobject
) {
    jstring hello = (jstring) "ddddd";
    return hello;
}


