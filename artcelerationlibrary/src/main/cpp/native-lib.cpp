//
// Created by Jianan on 11/4/2016.
//


#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <android/bitmap.h>
#include <cstring>
#include <unistd.h>
#include <string>

#define  LOG_TAG    "DEBUG"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

extern "C"
{
JNIEXPORT jstring JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_myStringFromJNI(JNIEnv *env, jobject /* this */);
JNIEXPORT jobject JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniStoreBitmapData(JNIEnv * env, jobject obj, jobject bitmap);
JNIEXPORT jobject JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniGetBitmapFromStoredBitmapData(JNIEnv * env, jobject obj, jobject handle);
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniFreeBitmapData(JNIEnv * env, jobject obj, jobject handle);
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniRotateBitmapCcw90(JNIEnv * env, jobject obj, jobject handle);
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniColorFilter(JNIEnv * env, jobject obj, jobject handle);
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniCropBitmap(JNIEnv * env, jobject obj, jobject handle, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);
JNIEXPORT jboolean JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_brightness(JNIEnv * env, jobject  obj, jobject bitmap, jfloat brightnessValue);

}

JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniColorFilter(JNIEnv * env, jobject obj, jobject handle)
{

}

JNIEXPORT jstring JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_myStringFromJNI(JNIEnv *env, jobject /* this */)
{
    std::string hello = "Hello";
    return env->NewStringUTF(hello.c_str());
}

class JniBitmap {
public:
    uint32_t* _storedBitmapPixels;
    AndroidBitmapInfo _bitmapInfo;
    JniBitmap()
    {
        _storedBitmapPixels = NULL;
    }
};

/**crops the bitmap within to be smaller. note that no validations are done*/ //
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniCropBitmap(JNIEnv * env, jobject obj, jobject handle, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
{
    JniBitmap* jniBitmap = (JniBitmap*) env->GetDirectBufferAddress(handle);
    if (jniBitmap->_storedBitmapPixels == NULL)
        return;
    uint32_t* previousData = jniBitmap->_storedBitmapPixels;
    uint32_t oldWidth = jniBitmap->_bitmapInfo.width;
    uint32_t newWidth = right - left, newHeight = bottom - top;
    uint32_t* newBitmapPixels = new uint32_t[newWidth * newHeight];
    uint32_t* whereToGet = previousData + left + top * oldWidth;
    uint32_t* whereToPut = newBitmapPixels;
    for (int y = top; y < bottom; ++y)
    {
        memcpy(whereToPut, whereToGet, sizeof(uint32_t) * newWidth);
        whereToGet += oldWidth;
        whereToPut += newWidth;
    }
    //done copying , so replace old data with new one
    delete[] previousData;
    jniBitmap->_storedBitmapPixels = newBitmapPixels;
    jniBitmap->_bitmapInfo.width = newWidth;
    jniBitmap->_bitmapInfo.height = newHeight;
}

/**rotates the inner bitmap data by 90 degress counter clock wise*/ //
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniRotateBitmapCcw90(JNIEnv * env, jobject obj, jobject handle)
{
    JniBitmap* jniBitmap = (JniBitmap*) env->GetDirectBufferAddress(handle);
    if (jniBitmap->_storedBitmapPixels == NULL)
        return;
    uint32_t* previousData = jniBitmap->_storedBitmapPixels;
    AndroidBitmapInfo bitmapInfo = jniBitmap->_bitmapInfo;
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
    jniBitmap->_storedBitmapPixels = newBitmapPixels;
    uint32_t temp = bitmapInfo.width;
    bitmapInfo.width = bitmapInfo.height;
    bitmapInfo.height = temp;
}

/**free bitmap*/  //
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniFreeBitmapData(JNIEnv * env, jobject obj, jobject handle)
{
    JniBitmap* jniBitmap = (JniBitmap*) env->GetDirectBufferAddress(handle);
    if (jniBitmap->_storedBitmapPixels == NULL)
        return;
    delete[] jniBitmap->_storedBitmapPixels;
    jniBitmap->_storedBitmapPixels = NULL;
    delete jniBitmap;
}

/**restore java bitmap (from JNI data)*/  //
JNIEXPORT jobject JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniGetBitmapFromStoredBitmapData(JNIEnv * env, jobject obj, jobject handle)
{
    JniBitmap* jniBitmap = (JniBitmap*) env->GetDirectBufferAddress(handle);
    if (jniBitmap->_storedBitmapPixels == NULL)
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
    jobject newBitmap = env->CallStaticObjectMethod(bitmapCls, createBitmapFunction, jniBitmap->_bitmapInfo.width, jniBitmap->_bitmapInfo.height, bitmapConfig);
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
    int pixelsCount = jniBitmap->_bitmapInfo.height * jniBitmap->_bitmapInfo.width;
    memcpy(newBitmapPixels, jniBitmap->_storedBitmapPixels, sizeof(uint32_t) * pixelsCount);
    AndroidBitmap_unlockPixels(env, newBitmap);
    //LOGD("returning the new bitmap");
    return newBitmap;
}

/**store java bitmap as JNI data*/  //
JNIEXPORT jobject JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniStoreBitmapData(JNIEnv * env, jobject obj, jobject bitmap)
{
    AndroidBitmapInfo bitmapInfo;
    uint32_t* storedBitmapPixels = NULL;
    //LOGD("reading bitmap info...");
    int ret;
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo)) < 0)
    {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return NULL;
    }
    LOGD("width:%d height:%d stride:%d", bitmapInfo.width, bitmapInfo.height, bitmapInfo.stride);
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
    storedBitmapPixels = new uint32_t[bitmapInfo.height * bitmapInfo.width];
    int pixelsCount = bitmapInfo.height * bitmapInfo.width;
    memcpy(storedBitmapPixels, src, sizeof(uint32_t) * pixelsCount);
    AndroidBitmap_unlockPixels(env, bitmap);
    JniBitmap *jniBitmap = new JniBitmap();
    jniBitmap->_bitmapInfo = bitmapInfo;
    jniBitmap->_storedBitmapPixels = storedBitmapPixels;
    return env->NewDirectByteBuffer(jniBitmap, 0);
}

static int rgb_clamp(int value) {
    if(value > 255) {
        return 255;
    }
    if(value < 0) {
        return 0;
    }
    return value;
}

static void brightness(AndroidBitmapInfo* info, void* pixels, float brightnessValue){
    int xx, yy, red, green, blue;
    uint32_t* line;

    for(yy = 0; yy < info->height; yy++){
        line = (uint32_t*)pixels;
        for(xx =0; xx < info->width; xx++){

            //extract the RGB values from the pixel
            red = (int) ((line[xx] & 0x00FF0000) >> 16);
            green = (int)((line[xx] & 0x0000FF00) >> 8);
            blue = (int) (line[xx] & 0x00000FF );

            //manipulate each value
            red = rgb_clamp((int)(red * brightnessValue));
            green = rgb_clamp((int)(green * brightnessValue));
            blue = rgb_clamp((int)(blue * brightnessValue));

            // set the new pixel back in
            line[xx] =
                    ((red << 16) & 0x00FF0000) |
                    ((green << 8) & 0x0000FF00) |
                    (blue & 0x000000FF);
        }

        pixels = (char*)pixels + info->stride;
    }
}


JNIEXPORT jboolean JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_brightness(JNIEnv * env, jobject  obj, jobject bitmap, jfloat brightnessValue)
{

    AndroidBitmapInfo  info;
    int ret;
    void* pixels;

    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return false;
    }
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format is not RGBA_8888 !");
        return false;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return false;
    }

    brightness(&info,pixels, brightnessValue);

    AndroidBitmap_unlockPixels(env, bitmap);
    return true;
}








