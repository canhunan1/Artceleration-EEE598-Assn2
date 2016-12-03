//
// Created by Jianan on 11/4/2016.
// This is the native lib file with native motion blur and native color filter transform
//


#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <android/bitmap.h>
#include <cstring>
#include <string>
#include <math.h>


#define  LOG_TAG    "DEBUG"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

int algo_ColorFilter(int inputColor, int inputParams[]);
void motionBlur(AndroidBitmapInfo* info, uint32_t * pixels, int dir, int radius);
void colorFilter(JNIEnv *env, jintArray args, const AndroidBitmapInfo &info, void *pixels);
void algo_GaussianBlue_first(uint32_t* pixels, int width, int intInput0, float* weightVector, int weightLength,int positionX, int positionY);
void algo_GaussianBlue_second(uint32_t* pixels, int height, int intInput0,float weightVector[], int weightLength, int positionX, int positionY) ;
void GaussianBlur(float * weightVector,uint32_t *pixels,int width,int height, int inputInt,float inputFloat, int weightLength);
extern "C"
{
    JNIEXPORT jstring JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_myStringFromJNI(JNIEnv *env, jobject  /*this*/ );
    JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniColorFilter(JNIEnv * env, jobject  obj, jobject bitmap, jintArray args, uint32_t size);
    JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniGaussianBlur(JNIEnv * env, jobject  obj, jobject bitmap, jintArray intArgs, jfloatArray floatArgs);
    JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_nativeMotionBlur(JNIEnv *env, jobject obj, jobject bitmap, jintArray args);
}
/*
 * This function is the test function
 * */
JNIEXPORT jstring JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_myStringFromJNI(JNIEnv *env, jobject /* this */)
{
       std::string hello = "Hello";
    return env->NewStringUTF(hello.c_str());
}

/*
 * This is the color filter transform which uses the same algorithm as in the java
 * This function is used to transfer the bitmap to byte array that can be used in cpp file
 * @param bitmap    bitmap is the image to be processed
 * @param args      args is the input argument
 * */
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
/*
 * This function is used to do the color filter transform
 * @param info      is the bitmap information in the cpp file
 * @param pixels    pixels is the pointer points to the first address of the bitmap.
 * */
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

/*
 * This is the function is used to do the color filter transform for a pixel
 * @inputParams   the input argument to compute the new pixel
 * @inputColor   the input color value for mathematical computation
 * @return      it returns a int value for a coler channel
 * */
int algo_ColorFilter(int inputColor, int inputParams[]) {
    // case 1: the input arguments are at neither 0 nor 255 boundary
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
    // case 2: the first element of input arguments is at 0 while the last element is not at 255
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

    // case 3: the first element of input arguments is not 0 while the last element is 255
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

    // case 4: the first and last element of input arguments are both at boundary, i.e. 0 and 255 respectively.
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
 * This is the motion blur transform which uses the same algorithm as in the java
 * This function is used to transfer the bitmap to byte array that can be used in cpp file
 * @param bitmap    bitmap is the image to be processed
 * @param args      args is the input argument
 * */
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_nativeMotionBlur(
        JNIEnv *env, jobject obj, jobject bitmap, jintArray args)
{
    jint *inCArray = env->GetIntArrayElements(args, NULL);
    LOGD("i1 = %d",inCArray[0]);
    LOGD("i2 = %d",inCArray[1]);
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

    //brightness(&info,pixels, fArgu);
    motionBlur(&info,(uint32_t*)pixels, inCArray[0], inCArray[1]);
    AndroidBitmap_unlockPixels(env, bitmap);
}
/*
 * This function is used to do the native motion blur
 * @param pixels   is the pointer points to the first address of the image
 * @dir            dir is the direction of the motion blur
 * @radius         radius is the radius of the motion blur
 * */
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



/*Gaussian Blur*/
JNIEXPORT void JNICALL Java_edu_asu_msrs_artcelerationlibrary_NativeTransform_jniGaussianBlur(JNIEnv * env, jobject  obj, jobject bitmap, jintArray intArgs, jfloatArray floatArgs)
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

    int * inputInt = env->GetIntArrayElements(intArgs, NULL);
    float * inputFloat = env->GetFloatArrayElements(floatArgs, NULL);
    int weightLength = 2 * inputInt[0] + 1;
    float * weightVector =(float*)malloc(sizeof(float)*weightLength);
    LOGD("Good 383");
    GaussianBlur(weightVector,(uint32_t*) pixels,info.width,info.height, inputInt[0],inputFloat[0],  weightLength);
    LOGD("Good 385");
    free(weightVector);
    AndroidBitmap_unlockPixels(env, bitmap);
}

/**
     * This function is used to start the Gaussian Blur transform, it takes no inputs and returns the transformed bitmap.
     */
void GaussianBlur(float * weightVector,uint32_t *pixels,int width,int height, int inputInt,float inputFloat, int weightLength) {
    //initiate Gaussian weight vector
   // weightInit(this.inputInt[0], inputFloat[0]);
    LOGD("height=%d, width=%d",width,height);
    for (int i = 0; i < 2 * inputInt + 1; i++) {

        float part_1 = (float) (1 / sqrt(2 * 3.14 * inputFloat * inputFloat));
        float expInput = -(-inputInt + i) * (-inputInt + i) / (2 * inputFloat * inputFloat);

        weightVector[i] = (float) (part_1 * exp(expInput));
    }
    LOGD("Good 403");
    //do the first half of Gaussian transform, that is for all pixels aligned in 'X-axis'
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            algo_GaussianBlue_first( pixels,  width,  inputInt,  weightVector, weightLength, x, y);
        }
    }
    LOGD("Good 410");
    //do the second half of Gaussian transform, that is for all pixels aligned in 'Y-axis'
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
             algo_GaussianBlue_second(pixels,  height,  inputInt, weightVector,  weightLength,  x,  y);
        }
    }
    LOGD("Good 417");
}

/**
 * This function is used to perform the first half of Gaussian Blur transform, it returns the calculated int value q.
 *
 * @param weightVector, is the Gassian weight vector
 * @param color         is the color to be transformed with red:0, green:1, blue:2
 * @param positionX,    is the pixel's X coordinate
 * @param positionY,    is the pixel's Y coordinate
 */

void algo_GaussianBlue_first(uint32_t* pixels, int width, int intInput0, float* weightVector, int weightLength,int positionX, int positionY) {
    uint32_t red ;
    uint32_t green ;
    uint32_t blue ;
    for (int i = 0; i < weightLength; i++) {

        if (positionX - intInput0 + i >= 0 &&
            positionX - intInput0 + i < width) {
            red =  (uint32_t)weightVector[i] *(uint32_t)((pixels[positionX - intInput0 + i+width*positionY] & 0x00FF0000) >> 16);
            green = (uint32_t)weightVector[i] *(uint32_t)((pixels[positionX - intInput0 + i+width*positionY] & 0x0000FF00) >> 8);
            blue = (uint32_t) weightVector[i] *(uint32_t)(pixels[positionX - intInput0 + i+width*positionY] & 0x000000FF );

            pixels[positionX - intInput0 + i+width*positionY] = (0xFF000000)|
                                ((red << 16) & 0x00FF0000) |
                                ((green << 8) & 0x0000FF00) |
                                (blue & 0x000000FF);
        } else {
            pixels[positionX - intInput0 + i+width*positionY] = (0xFF000000)|
                                                                ((0 << 16) & 0x00FF0000) |
                                                                ((0 << 8) & 0x0000FF00) |
                                                                (0 & 0x000000FF);
        }
    }
}

/**
 * This function is used to perform the second half of Gaussian Blur transform, it returns the calculated int value p.
 *
 * @param weightVector, is the Gassian weight vector
 * @param color         is the color to be transformed with red:0, green:1, blue:2
 * @param positionX,    is the pixel's X coordinate
 * @param positionY,    is the pixel's Y coordinate
 */

void algo_GaussianBlue_second(uint32_t* pixels, int height, int intInput0,float weightVector[], int weightLength, int positionX, int positionY) {
    uint32_t red ;
    uint32_t green ;
    uint32_t blue ;
    for (int i = 0; i < weightLength; i++) {

        if (positionY - intInput0 + i >= 0 &&
            positionY - intInput0 + i < height) {
            red =  (uint32_t)weightVector[i] *(uint32_t)((pixels[positionX*height+positionY - intInput0 + i] & 0x00FF0000) >> 16);
            green = (uint32_t)weightVector[i] *(uint32_t)((pixels[positionX*height+positionY - intInput0 + i] & 0x0000FF00) >> 8);
            blue = (uint32_t) weightVector[i] *(uint32_t)(pixels[positionX*height+positionY - intInput0 + i] & 0x000000FF );

            pixels[positionX*height+positionY - intInput0 + i] = (0xFF000000)|
                                                                ((red << 16) & 0x00FF0000) |
                                                                ((green << 8) & 0x0000FF00) |
                                                                (blue & 0x000000FF);
        } else {
            pixels[positionX*height+positionY - intInput0 + i] = (0xFF000000)|
                                                                 ((0 << 16) & 0x00FF0000) |
                                                                 ((0 << 8) & 0x0000FF00) |
                                                                 (0 & 0x000000FF);
        }
    }
}






