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

int algo_ColorFilter(int inputColor, int inputParams[]);
void motionBlur(AndroidBitmapInfo* info, uint32_t * pixels, int dir, int radius);

void colorFilter(JNIEnv *env, const _jintArray *args, const AndroidBitmapInfo &info, void *pixels);
extern "C"
{
JNIEXPORT jstring JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_myStringFromJNI(JNIEnv *env, jobject  /*this*/ );
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_neonMotionBlur(JNIEnv * env, jobject  obj, jobject bitmap, jfloat brightnessValue);
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniColorFilter(JNIEnv * env, jobject  obj, jobject bitmap, jintArray args, uint32_t size);;
}



JNIEXPORT jstring JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_myStringFromJNI(JNIEnv *env, jobject /* this */)
{
    //fir_filter_neon_intrinsics(fir_output, fir_input, fir_kernel, FIR_OUTPUT_SIZE, FIR_KERNEL_SIZE);
    //int16x4_t  input_vec = vld1_s16(fir_input);

    std::string hello = "Hello";
    return env->NewStringUTF(hello.c_str());
}

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







