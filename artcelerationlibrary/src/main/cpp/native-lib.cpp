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
//JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniColorFilter(JNIEnv * env, jobject obj, jobject handle, jintArray args, uint32_t size);
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniCropBitmap(JNIEnv * env, jobject obj, jobject handle, uint32_t left, uint32_t top, uint32_t right, uint32_t bottom);
JNIEXPORT jboolean JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_brightness(JNIEnv * env, jobject  obj, jobject handle,jfloat brightnessValue);
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_neonMotionBlur(JNIEnv * env, jobject  obj, jobject bitmap, jfloat brightnessValue);
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniColorFilter(JNIEnv * env, jobject  obj, jobject bitmap, jintArray args, uint32_t size);;


}
#ifdef __cplusplus /* If this is a C++ compiler, use C linkage */
extern "C" {
#endif
//void fir_filter_neon_intrinsics(short *output, const short* input, const short* kernel, int width, int kernelSize);

#ifdef __cplusplus /* If this is a C++ compiler, end C linkage */
}
#endif




void motionBlur(AndroidBitmapInfo* info, uint32_t * pixels, int dir, int radius);

void colorFilter(JNIEnv *env, const _jintArray *args, const AndroidBitmapInfo &info, void *pixels);

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


void colorFilter(JNIEnv *env, jintArray args, const AndroidBitmapInfo &info, void *pixels) {
    jint *inCArray = env->GetIntArrayElements(args, NULL);

    uint32_t height = info.height;
    uint32_t width = info.width;
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
}
void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniColorFilter(JNIEnv * env, jobject  obj, jobject bitmap, jintArray args, uint32_t size)
{

    AndroidBitmapInfo  info;
    int ret;
    void* pixels;
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format is not RGBA_8888 !");
        return;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    colorFilter(env, args, info, pixels);

    AndroidBitmap_unlockPixels(env, bitmap);
}


/*JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniColorFilter(JNIEnv * env, jobject obj, jobject handle, jintArray args, uint32_t size)
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
}*/
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

/*
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
*/



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






JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_neonMotionBlur(JNIEnv * env, jobject  obj, jobject bitmap, jfloat brightnessValue)
{

    AndroidBitmapInfo  info;
    int ret;
    void* pixels;
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
        return;
    }
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("Bitmap format is not RGBA_8888 !");
        return;
    }

    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &pixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    //brightness(&info,pixels, brightnessValue);
    motionBlur(&info,(uint32_t*)pixels, 1, 20);
    AndroidBitmap_unlockPixels(env, bitmap);
}

void motionBlur(AndroidBitmapInfo* info, uint32_t * pixels, int dir, int radius){
    uint32_t  width = info->width;
    uint32_t height = info->height;
    uint32_t* line;
    int pixelsCount = width * height;
    uint32_t* ori=  (uint32_t*)malloc(sizeof(uint32_t) *pixelsCount);
    memcpy(ori, pixels, sizeof(uint32_t) * pixelsCount);
    if(dir == 0){// horizontal blur
        for( int i = 0; i < info->height; i++){
            line = (uint32_t*) pixels;
            uint32_t sumRed = 0;
            uint32_t sumGreen = 0;
            uint32_t sumBlue = 0;
            for(int j = 0; j<2*radius+1;j++){
                sumRed +=  (uint32_t)(( line[j] & 0x00FF0000) >> 16);
                sumGreen += (uint32_t)(( line[j] & 0x0000FF00) >> 8);
                sumBlue +=  (uint32_t)( line[j] & 0x000000FF );
                if(radius<=j){
                    uint32_t  red = (sumRed/(j+1));
                    uint32_t  green = (sumGreen/(j+1));
                    uint32_t  blue = (sumBlue/(j+1));
                    line[j-radius] = (0xFF000000)|
                                     ((red << 16) & 0x00FF0000) |
                                     ((green << 8) & 0x0000FF00) |
                                     (blue & 0x000000FF);
                    /* LOGE("%d  green = %d", j-radius,(line[j-radius]& 0x0000FF00)>> 8);
                     //LOGE("  sum green = %d", sumGreen);
                     LOGE("%d blue = %d", j-radius,(line[j-radius]& 0x000000FF));
                     //LOGE("   sum blue = %d", sumBlue);*/
                }

                /*LOGE("%d  green = %d", j,(line[j]& 0x0000FF00)>> 8);
                LOGE("  sum green = %d", sumGreen);
                LOGE("%d blue = %d", j,(line[j]& 0x000000FF));
                LOGE("   sum blue = %d", sumBlue);*/

            }
            for(int j =radius + 1; j <  info->width-radius; j++){
                //extract the RGB values from the pixel
                uint32_t pixelRight =  line[ j + radius];
                uint32_t pixelLeft = line[ j - radius - 1];// should use ori but have bug here
                sumRed +=  (uint32_t)(( pixelRight & 0x00FF0000) >> 16) - (uint32_t)(( pixelLeft & 0x00FF0000) >> 16);
                sumGreen += (uint32_t)(( pixelRight & 0x0000FF00) >> 8) - (uint32_t)(( pixelLeft & 0x0000FF00) >> 8);
                sumBlue +=  (uint32_t)( pixelRight & 0x000000FF ) - (uint32_t)(( pixelLeft & 0x000000FF));
                uint32_t  red = (sumRed/(2*radius+1));
                uint32_t  green = (sumGreen/(2*radius+1));
                uint32_t  blue = (sumBlue/(2*radius+1));
                // set the new pixel back in
                /* LOGE("%d green = %d", j,(ori[j]& 0x0000FF00)>> 8);
                 LOGE("   sum green = %d", sumGreen);
                 LOGE("   aver green = %d", green);
                 LOGE("%d blue = %d", j,(ori[j]& 0x000000FF));
                 LOGE("   sum blue = %d", sumBlue);
                 LOGE("   aver blue = %d", blue);*/
                line[j] = (0xFF000000)|
                          ((red << 16) & 0x00FF0000) |
                          ((green << 8) & 0x0000FF00) |
                          (blue & 0x000000FF);
            }
            pixels = (uint32_t*)((char*)pixels + info->stride);
        }
    }else{// vertical blur
        for( int i = 0; i < width; i++){
            //line = (uint32_t*) pixels;
            uint32_t sumRed = 0;
            uint32_t sumGreen = 0;
            uint32_t sumBlue = 0;
            for(int j = 0; j<2*radius+1;j++){
                sumRed +=  (uint32_t)(( pixels[j*width+i] & 0x00FF0000) >> 16);
                sumGreen += (uint32_t)(( pixels[j*width+i] & 0x0000FF00) >> 8);
                sumBlue +=  (uint32_t)( pixels[j*width+i] & 0x000000FF );
                if(radius<=j){
                    uint32_t  red = (sumRed/(j+1));
                    uint32_t  green = (sumGreen/(j+1));
                    uint32_t  blue = (sumBlue/(j+1));
                    pixels[(j-radius)*width+i] = (0xFF000000)|
                                                 ((red << 16) & 0x00FF0000) |
                                                 ((green << 8) & 0x0000FF00) |
                                                 (blue & 0x000000FF);
                    /* LOGE("%d  green = %d", j-radius,(line[j-radius]& 0x0000FF00)>> 8);
                     //LOGE("  sum green = %d", sumGreen);
                     LOGE("%d blue = %d", j-radius,(line[j-radius]& 0x000000FF));
                     //LOGE("   sum blue = %d", sumBlue);*/
                }

                /*LOGE("%d  green = %d", j,(line[j]& 0x0000FF00)>> 8);
                LOGE("  sum green = %d", sumGreen);
                LOGE("%d blue = %d", j,(line[j]& 0x000000FF));
                LOGE("   sum blue = %d", sumBlue);*/

            }
            for(int j =radius + 1; j <  height-radius; j++){
                //extract the RGB values from the pixel
                uint32_t pixelRight =  pixels[ (j+ radius)*width+i ];
                uint32_t pixelLeft = pixels[ (j- radius - 1)*width+i ];// should use ori but have bug here
                sumRed +=  (uint32_t)(( pixelRight & 0x00FF0000) >> 16) - (uint32_t)(( pixelLeft & 0x00FF0000) >> 16);
                sumGreen += (uint32_t)(( pixelRight & 0x0000FF00) >> 8) - (uint32_t)(( pixelLeft & 0x0000FF00) >> 8);
                sumBlue +=  (uint32_t)( pixelRight & 0x000000FF ) - (uint32_t)(( pixelLeft & 0x000000FF));
                uint32_t  red = (sumRed/(2*radius+1));
                uint32_t  green = (sumGreen/(2*radius+1));
                uint32_t  blue = (sumBlue/(2*radius+1));
                // set the new pixel back in
                /* LOGE("%d green = %d", j,(ori[j]& 0x0000FF00)>> 8);
                 LOGE("   sum green = %d", sumGreen);
                 LOGE("   aver green = %d", green);
                 LOGE("%d blue = %d", j,(ori[j]& 0x000000FF));
                 LOGE("   sum blue = %d", sumBlue);
                 LOGE("   aver blue = %d", blue);*/
                pixels[j*width+i] = (0xFF000000)|
                                    ((red << 16) & 0x00FF0000) |
                                    ((green << 8) & 0x0000FF00) |
                                    (blue & 0x000000FF);
            }
            //pixels = (char*)pixels + sizeof(uint32_t)*width;
        }
    }

    free(ori);
}







