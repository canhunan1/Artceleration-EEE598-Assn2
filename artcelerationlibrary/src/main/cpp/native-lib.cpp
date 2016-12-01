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
#include <cpu-features.h>
//#include <arm_neon.h>
#include "neon-intrinsics.h"

#define  LOG_TAG    "DEBUG"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
static uint32_t rgb_clamp(uint32_t value);
static uint32_t monoColorFilter(uint32_t monoColor, uint32_t args[]);
int algo_ColorFilter(int inputColor, int inputParams[]);
extern "C"
{
JNIEXPORT jstring JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_myStringFromJNI(JNIEnv *env, jobject  /*this*/ );
JNIEXPORT jobject JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniStoreBitmapData(JNIEnv * env, jobject obj, jobject bitmap);
JNIEXPORT jobject JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniGetBitmapFromStoredBitmapData(JNIEnv * env, jobject obj, jobject handle);
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniFreeBitmapData(JNIEnv * env, jobject obj, jobject handle);
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniRotateBitmapCcw90(JNIEnv * env, jobject obj, jobject handle);
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniColorFilter(JNIEnv * env, jobject obj, jobject handle, jintArray args, uint32_t size);
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniCropBitmap(JNIEnv * env, jobject obj, jobject handle, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);
JNIEXPORT jboolean JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_brightness(JNIEnv * env, jobject  obj, jobject handle,jfloat brightnessValue);

}
#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif
//void fir_filter_neon_intrinsics(short *output, const short* input, const short* kernel, int width, int kernelSize);

#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif

#define  FIR_KERNEL_SIZE   32
#define  FIR_OUTPUT_SIZE   2560
#define  FIR_INPUT_SIZE    (FIR_OUTPUT_SIZE + FIR_KERNEL_SIZE)
#define  FIR_ITERATIONS    600

static const short fir_kernel[FIR_KERNEL_SIZE] = {
        0x10, 0x20, 0x40, 0x70, 0x8c, 0xa2, 0xce, 0xf0, 0xe9, 0xce, 0xa2, 0x8c, 070, 0x40, 0x20,
        0x10,
        0x10, 0x20, 0x40, 0x70, 0x8c, 0xa2, 0xce, 0xf0, 0xe9, 0xce, 0xa2, 0x8c, 070, 0x40, 0x20,
        0x10};

static short fir_output[FIR_OUTPUT_SIZE];
static short fir_input_0[FIR_INPUT_SIZE];
static const short *fir_input = fir_input_0 + (FIR_KERNEL_SIZE / 2);

JNIEXPORT jstring JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_myStringFromJNI(JNIEnv *env, jobject /* this */)
{
    //fir_filter_neon_intrinsics(fir_output, fir_input, fir_kernel, FIR_OUTPUT_SIZE, FIR_KERNEL_SIZE);
    //int16x4_t  input_vec = vld1_s16(fir_input);

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
class Color{
public:
    uint32_t red;
    uint32_t green;
    uint32_t blue;
    /*Color(uint32_t red, uint32_t green, uint32_t blue){
        this->red = red;
        this->green = green;
        this->blue = blue;
    }*/
};
void setColor(uint32_t* pixel, Color color){
    uint32_t red = color.red;
    uint32_t green = color.green;
    uint32_t blue = color.blue;
    *pixel= (0xFF000000) |
            ((red << 16)  & 0x00FF0000) |
            ((green << 8)  & 0x0000FF00) |
            (blue  & 0x000000FF);
}

Color getColor(uint32_t* pixel){
    Color color;
    color.red =  (uint32_t)((*pixel & 0x00FF0000) >> 16);
    color.green = (uint32_t)((*pixel & 0x0000FF00) >> 8);
    color.blue =  (uint32_t)(*pixel & 0x000000FF );
    return color;
}


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

JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniColorFilter(JNIEnv * env, jobject obj, jobject handle, jintArray args, uint32_t size)
{
    jint *inCArray = env->GetIntArrayElements(args,NULL);

    JniBitmap* jniBitmap = (JniBitmap*) env->GetDirectBufferAddress(handle);
    if (jniBitmap->_storedBitmapPixels == NULL)
        return;
    AndroidBitmapInfo info = jniBitmap->_bitmapInfo;
    uint32_t* previousData = jniBitmap->_storedBitmapPixels;
    uint32_t height = jniBitmap->_bitmapInfo.height;
    uint32_t width = jniBitmap->_bitmapInfo.width;
    //uint32_t* newBitmapPixels = new uint32_t[width * height];

    void* pixels = (void*)previousData;

    uint32_t xx, yy, red, green, blue;
    uint32_t* line;

    for(yy = 0; yy < height; yy++){
        line = (uint32_t*)pixels;
        for(xx =0; xx < width; xx++){
            //extract the RGB values from the pixel
            red =  (uint32_t)((line[xx] & 0x00FF0000) >> 16);
            green = (uint32_t)((line[xx] & 0x0000FF00) >> 8);
            blue =  (uint32_t)(line[xx] & 0x000000FF );

           //manipulate each value
            red = (uint32_t) algo_ColorFilter(red,inCArray);
            green = (uint32_t) algo_ColorFilter(green,(inCArray+8));
            blue = (uint32_t)algo_ColorFilter(blue,(inCArray+16));

            // set the new pixel back in
            line[xx]=
                    ( 0xFF000000) |
                    ((red << 16)  & 0x00FF0000) |
                    ((green << 8)  & 0x0000FF00) |
                    (blue  & 0x000000FF);
        }
        pixels = (char*)pixels + info.stride; //yy*width;//
    }//

    jniBitmap->_storedBitmapPixels = previousData;
    jniBitmap->_bitmapInfo.width = info.width;
    jniBitmap->_bitmapInfo.height = info.height;
}
int algo_ColorFilter(int inputColor, int inputParams[]) {
    if (inputParams[0] != 0 && inputParams[6] != 255) {
        if (inputColor < inputParams[0]) {

            inputColor = inputParams[1] / inputParams[0] * inputColor;

        } else if (inputColor >= inputParams[0] && inputColor < inputParams[2]) {

            inputColor = (inputParams[3] - inputParams[1]) / (inputParams[2] - inputParams[0]) * (inputColor - inputParams[0]) + inputParams[1];

        } else if (inputColor >= inputParams[2] && inputColor < inputParams[4]) {

            inputColor = (inputParams[5] - inputParams[3]) / (inputParams[4] - inputParams[2]) * (inputColor - inputParams[2]) + inputParams[3];

        } else if (inputColor >= inputParams[4] && inputColor < inputParams[6]) {

            inputColor = (inputParams[7] - inputParams[5]) / (inputParams[6] - inputParams[4]) * (inputColor - inputParams[4]) + inputParams[5];

        } else {

            inputColor = (255 - inputParams[7]) / (255 - inputParams[6]) * (inputColor - inputParams[6]) + inputParams[7];
        }

    } else if (inputParams[0] == 0 && inputParams[6] != 255) {

        if (inputColor >= inputParams[0] && inputColor < inputParams[2]) {

            inputColor = (inputParams[3] - inputParams[1]) / (inputParams[2] - inputParams[0]) * (inputColor - inputParams[0]) + inputParams[1];

        } else if (inputColor >= inputParams[2] && inputColor < inputParams[4]) {

            inputColor = (inputParams[5] - inputParams[3]) / (inputParams[4] - inputParams[2]) * (inputColor - inputParams[2]) + inputParams[3];

        } else if (inputColor >= inputParams[4] && inputColor < inputParams[6]) {

            inputColor = (inputParams[7] - inputParams[5]) / (inputParams[6] - inputParams[4]) * (inputColor - inputParams[4]) + inputParams[5];

        } else {

            inputColor = (255 - inputParams[7]) / (255 - inputParams[6]) * (inputColor - inputParams[6]) + inputParams[7];
        }

    } else if (inputParams[0] != 0) {

        if (inputColor < inputParams[0]) {

            inputColor = inputParams[1] / inputParams[0] * inputColor;

        } else if (inputColor >= inputParams[0] && inputColor < inputParams[2]) {

            inputColor = (inputParams[3] - inputParams[1]) / (inputParams[2] - inputParams[0]) * (inputColor - inputParams[0]) + inputParams[1];

        } else if (inputColor >= inputParams[2] && inputColor < inputParams[4]) {

            inputColor = (inputParams[5] - inputParams[3]) / (inputParams[4] - inputParams[2]) * (inputColor - inputParams[2]) + inputParams[3];

        } else if (inputColor >= inputParams[4] && inputColor <= 255) {

            inputColor = (inputParams[7] - inputParams[5]) / (255 - inputParams[4]) * (inputColor - inputParams[4]) + inputParams[5];

        }

    } else {

        if (inputColor >= inputParams[0] && inputColor < inputParams[2]) {

            inputColor = (inputParams[3] - inputParams[1]) / (inputParams[2] - inputParams[0]) * (inputColor - inputParams[0]) + inputParams[1];

        } else if (inputColor >= inputParams[2] && inputColor < inputParams[4]) {

            inputColor = (inputParams[5] - inputParams[3]) / (inputParams[4] - inputParams[2]) * (inputColor - inputParams[2]) + inputParams[3];

        } else if (inputColor >= inputParams[4] && inputColor < 255) {

            inputColor = (inputParams[7] - inputParams[5]) / (255 - inputParams[4]) * (inputColor - inputParams[4]) + inputParams[5];

        }

    }

    return inputColor;
}

uint32_t monoColorFilter(uint32_t monoColor, uint32_t args[]) {
    // the size of the args should be
    if(args[0]>0 && monoColor<=args[0])
        return monoColor * args[1]/args[0];
    if(args[6]<255 && monoColor >=args[6])
        return (255-args[7])/(255-args[6])*(monoColor-args[6])+args[7];

    for (int i = 2; i < 8; i += 2) {
        if (args[i - 2] <= monoColor && args[i] >= monoColor) {
            monoColor = (args[i+1] - args[i-1]) / (args[i] - args[i-2]) * (monoColor - args[i-2]) + args[i-1];
            break;
        }
    }
    return monoColor;
}

/**rotates the inner bitmap data by 90 degress counter clock wise*/ //
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniRotateBitmapCcw90(JNIEnv * env, jobject obj, jobject handle)
{
    JniBitmap* jniBitmap = (JniBitmap*) env->GetDirectBufferAddress(handle);
    if (jniBitmap->_storedBitmapPixels == NULL) {
        LOGE("AndroidBitmap_storedBitmapPixels == NULL !");
        return;
    }
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
        LOGE("getBitmap_AndroidBitmap_lockPixels() failed ! error=%d", ret);
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
        LOGE("store_AndroidBitmap_lockPixels() failed ! error=%d", ret);
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

static uint32_t rgb_clamp(uint32_t value) {
    if(value > 255) {
        return 255;
    }
    if(value < 0) {
        return 0;
    }
    return value;
}

static void brightness(AndroidBitmapInfo* info, void* pixels, float brightnessValue){
    uint32_t xx, yy, red, green, blue;
    uint32_t* line;

    for(yy = 0; yy < info->height; yy++){
        line = (uint32_t*)pixels;
        for(xx =0; xx < info->width; xx++){

            //extract the RGB values from the pixel
            red = ((line[xx] & 0x00FF0000) >> 16);
            green = ((line[xx] & 0x0000FF00) >> 8);
            blue =  (line[xx] & 0x000000FF );

            //manipulate each value
            red = rgb_clamp((uint32_t)(red * brightnessValue));
            green = rgb_clamp((uint32_t)(green * brightnessValue));
            blue = rgb_clamp((uint32_t)(blue * brightnessValue));

            // set the new pixel back in
            line[xx] =
                    (0xFF000000) |
                    ((red << 16) & 0x00FF0000) |
                    ((green << 8) & 0x0000FF00) |
                    (blue & 0x000000FF);
        }

        pixels = (char*)pixels + info->stride;
    }
}



JNIEXPORT jboolean JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_brightness(JNIEnv * env, jobject  obj, jobject handle, jfloat brightnessValue)
{

    /*AndroidBitmapInfo  info;
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
        LOGE("bright_AndroidBitmap_lockPixels() failed ! error=%d", ret);
        return false;
    }
      brightness(&info,pixels, brightnessValue);*/
    JniBitmap* jniBitmap = (JniBitmap*) env->GetDirectBufferAddress(handle);
    if (jniBitmap->_storedBitmapPixels == NULL)
        return false;
    uint32_t* previousData = jniBitmap->_storedBitmapPixels;
    AndroidBitmapInfo bitmapInfo = jniBitmap->_bitmapInfo;
    //uint32_t* newBitmapPixels = new uint32_t[bitmapInfo.height * bitmapInfo.width];
    brightness(&bitmapInfo,previousData, brightnessValue);



    jniBitmap->_storedBitmapPixels = previousData;
    uint32_t temp = bitmapInfo.width;
    bitmapInfo.width = bitmapInfo.height;
    bitmapInfo.height = temp;



    return true;
}








