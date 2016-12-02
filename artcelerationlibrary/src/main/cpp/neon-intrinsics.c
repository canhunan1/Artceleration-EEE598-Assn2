#include "neon-intrinsics.h"
#include <arm_neon.h>
#include <malloc.h>
#include <string.h>
#include <android/bitmap.h>
#include <android/log.h>

#define  LOG_TAG    "DEBUG"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
/* this source file should only be compiled by Android.mk /CMake when targeting
 * the armeabi-v7a ABI, and should be built in NEON mode
 */

void
fir_filter_neon_intrinsics(short *output, const short* input, const short* kernel, int width, int kernelSize)
{
#if 1
    int nn, offset = -kernelSize/2;

    for (nn = 0; nn < width; nn++)
    {
        int mm, sum = 0;
        int32x4_t sum_vec = vdupq_n_s32(0);
        for(mm = 0; mm < kernelSize/4; mm++)
        {
            int16x4_t  kernel_vec = vld1_s16(kernel + mm*4);
            int16x4_t  input_vec = vld1_s16(input + (nn+offset+mm*4));
            sum_vec = vmlal_s16(sum_vec, kernel_vec, input_vec);
        }

        sum += vgetq_lane_s32(sum_vec, 0);
        sum += vgetq_lane_s32(sum_vec, 1);
        sum += vgetq_lane_s32(sum_vec, 2);
        sum += vgetq_lane_s32(sum_vec, 3);

        if(kernelSize & 3)
        {
            for(mm = kernelSize - (kernelSize & 3); mm < kernelSize; mm++)
                sum += kernel[mm] * input[nn+offset+mm];
        }

        output[nn] = (short)((sum + 0x8000) >> 16);
    }
#else /* for comparison purposes only */
    int nn, offset = -kernelSize/2;
    for (nn = 0; nn < width; nn++) {
        int sum = 0;
        int mm;
        for (mm = 0; mm < kernelSize; mm++) {
            sum += kernel[mm]*input[nn+offset+mm];
        }
        output[n] = (short)((sum + 0x8000) >> 16);
    }
#endif
}

void color_filter_neon_intrinsics(short *output, const short* input, const short* kernel, int width, int kernelSize){

}

void neonMotionBlur(AndroidBitmapInfo* info, uint32_t * pixels, int dir, int radius){
    uint32_t  width = info->width;
    uint32_t height = info->height;
    uint32_t* line;
    int pixelsCount = width * height;
    uint32_t* ori=  malloc(sizeof(uint32_t) *pixelsCount);
    memcpy(ori, pixels, sizeof(uint32_t) * pixelsCount);

    if(dir == 0){// horizontal blur
        //width = width/8;
        height = height/8;
        for( int i = 0; i < height; i++){
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
        width = width/8;
        for( int i = 0; i < width; i++){
            //line = (uint32_t*) pixels;
            uint8x8x3_t argb  = vld3_u8 ((uint8_t *)pixels);
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

void neonNeonEdgeLinearSum(AndroidBitmapInfo* infoOri, uint32_t * pixelsOri,AndroidBitmapInfo* infoProcessed, uint32_t * pixelsProcessed,float f1,float f2){
    /*uint32_t  width = infoOri->width;
    uint32_t  height = infoOri->height;
    uint32x4_t redFactor = vdupq_n_u32(0x00FF0000);
    uint32x4_t greenFactor = vdupq_n_u32(0x0000FF00);
    uint32x4_t blueFactor = vdupq_n_u32(0x000000FF);
    float32x4_t f1V =  vmovq_n_f32((float32_t)f1);
    float32x4_t f2V =  vmovq_n_f32((float32_t)f2);
    uint32_t* linePro;
    uint32_t* lineOri;


    for(int i = 0; i<height; i++) {
        linePro = (uint32_t*) pixelsProcessed;
        lineOri = (uint32_t*) pixelsOri;
        for(int j = 0; j<width/4;j += 4) {

            uint32x4_t ori = vld1q_u32(&lineOri[j]);
            uint32x4_t redOri = vandq_u32(ori, redFactor);
            uint32x4_t greenOri = vandq_u32(ori, greenFactor);
            uint32x4_t blueOri = vandq_u32(ori, blueFactor);

            uint32x4_t pre = vld1q_u32(&linePro[j]);
            uint32x4_t redPre = vandq_u32(pre, redFactor);
            uint32x4_t greenPre = vandq_u32(pre, greenFactor);
            uint32x4_t bluePre = vandq_u32(pre, blueFactor);

            float32x4_t red = vmlaq_f32(vmulq_f32((float32_t)redPre , f1V), (float32_t)redOri, f2V);
            float32x4_t green = vmlaq_f32(vmulq_f32((float32_t)greenPre , f1V), (float32_t)greenOri, f2V);
            float32x4_t blue = vmlaq_f32(vmulq_f32((float32_t)bluePre , f1V), (float32_t)blueOri, f2V);


            //uint8x8_t a =  vdup_n_u8(0);
        }
        pixelsProcessed = (uint32_t*)((char*)pixelsProcessed + infoProcessed->stride);
        pixelsOri = (uint32_t*)((char*)pixelsOri + infoOri->stride);
    }*/


}

void neonTest(){
    uint8x8_t a =  vdup_n_u8(0);

    LOGD("result = %d" , a);
}